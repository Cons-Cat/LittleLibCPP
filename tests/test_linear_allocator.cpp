#include <cat/allocators>
#include <cat/array>
#include <cat/math>
#include <cat/numerals>
#include <cat/utility>

auto main() -> int {
    // Initialize an allocator.
    cat::PageAllocator paging_allocator;
    int4* p_page = paging_allocator.p_malloc<int4>(1_ki).or_exit();

    cat::LinearAllocator allocator(p_page, 24);
    // It should not be possible to allocate 7 times here, because 24 bytes can
    // only hold 6 `int4`s.
    for (int i = 0; i < 7; ++i) {
        cat::Optional handle = allocator.malloc<int4>();
        if (!handle.has_value()) {
            Result(i == 6).or_exit();
            goto didnt_overallocate;
        }
    }
    cat::exit(1);

didnt_overallocate:
    // Invalidate all memory handles, and allocate again.
    allocator.reset();
    for (int4 i = 0; i < 4; ++i) {
        cat::Optional handle = allocator.malloc();
        Result(handle.has_value()).or_exit();
    }
    // This allocated 16 bytes, which is 8-byte-aligned. Another int allocation
    // would make it 4-byte-aligned. However, 8 bytes should be allocated here
    // to keep it 8-byte-aligned.
    auto handle = allocator.aligned_alloc<int4>(8u).value();
    Result(cat::is_aligned(&allocator.get(handle), 8u)).or_exit();

    // Allocate another int.
    auto handle_2 = allocator.malloc<int4>().value();
    Result(cat::is_aligned(&allocator.get(handle_2), 4u)).or_exit();
    // This is now 4-byte-aligned.
    Result(!cat::is_aligned(&allocator.get(handle_2), 8u)).or_exit();

    // Small size allocations shouldn't bump the allocator.
    for (int4 i = 0; i < 20; ++i) {
        auto memory = allocator.malloca<int4>();
        Result(memory.has_value()).or_exit();
    }
    cat::Optional handle_3 = allocator.malloc<int4>();
    Result(handle_3.has_value()).or_exit();

    // Test that allocations are reusable.
    allocator.reset();
    decltype(allocator.malloc<int1>()) handles[4];
    for (signed char i = 0; i < 4; ++i) {
        handles[i] = allocator.malloc<int1>();
        Result(handles[i].has_value()).or_exit();
        allocator.get(handles[i].value()) = i;
    }
    for (signed char i = 0; i < 4; ++i) {
        Result(allocator.get(handles[i].value()) == i).or_exit();
    }

    // Test that allocations have pointer stability.
    allocator.reset();
    int4* p_handles[4];
    for (int i = 0; i < 4; ++i) {
        int4* p_handle = allocator.p_malloc<int4>().or_exit();
        *p_handle = i;
        p_handles[i] = p_handle;
    }
    for (int i = 0; i < 4; ++i) {
        Result(*(p_handles[i]) == i).or_exit();
        allocator.free(p_handles[i]).or_exit();
    }

    paging_allocator.free(p_page).or_exit();

    cat::exit();
}
