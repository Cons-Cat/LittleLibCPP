// -*- mode: c++ -*-
// vim: set ft=cpp:
#include <string.h>

// TODO: This should use an AnyConst container.
/* Copy some bytes from one address to another address. There exists
 * `simd::string_length_as<>()`. With the tree-loop-distribute-patterns
 * optimization, this generates a call to `memcpy()` which cannot be resolved,
 * so that must be disabled for this function, or it cannot be linked.
 */
// NOLINTNEXTLINE
[[gnu::optimize("-fno-tree-loop-distribute-patterns")]] void std::copy_memory(
    void const* p_source, void* p_destination, isize bytes) {
    unsigned char const* p_source_handle =
        static_cast<unsigned char const*>(p_source);
    unsigned char* p_destination_handle =
        static_cast<unsigned char*>(p_destination);

    for (isize i = 0; i < bytes; i++) {
        p_destination_handle[i] = p_source_handle[i];
    }
}

// TODO: Make integers consistently signed.
/* Copy some bytes from one address to another address. */
void simd::copy_memory(void const* p_source, void* p_destination, isize bytes) {
    // Vector is the width of a 32-byte AVX register.
    // `long long int` is required for some SIMD intrinsics.
    using VectorType = std::detail::SimdVector<long long int, 4>;

    unsigned char const* p_source_handle =
        meta::bit_cast<unsigned char const*>(p_source);
    unsigned char* p_destination_handle =
        meta::bit_cast<unsigned char*>(p_destination);
    constexpr isize cachesize = 0x200000;  // L3-cache size.
    isize padding;

    if (bytes <= 256) {
        std::copy_memory(p_source, p_destination, bytes);
    }

    // Align source, destination, and bytes to 16 bytes
    padding = (32 - ((meta::bit_cast<isize>(p_destination_handle)) & 31)) & 31;

    VectorType head = *meta::bit_cast<VectorType const*>(p_source_handle);
    *static_cast<VectorType*>(p_destination) = head;

    p_source_handle += padding;
    p_destination_handle += padding;
    bytes -= padding;

    VectorType vectors[8];
    constexpr isize step_size = sizeof(VectorType) * 8;
    // This routine is optimized for buffers in L3 cache. Streaming is slower.
    if (bytes <= cachesize) {
        while (bytes >= step_size) {
            /* Load 8 8x4 vectors, then increment the source pointer by that
             * size. */
#pragma GCC unroll 8
            for (i4 i = 0; i < 8; i++) {
                vectors[i] = meta::bit_cast<VectorType*>(p_source_handle)[i];
            }
            prefetch((char const*)(p_source_handle + 512), simd::MM_HINT_NTA);
#pragma GCC unroll 8
            for (i4 i = 0; i < 8; i++) {
                meta::bit_cast<VectorType*>(p_destination_handle)[i] =
                    vectors[i];
            }
            p_source_handle += step_size;
            p_destination_handle += step_size;
            bytes -= step_size;
        }
    }
    // This routine is run when the memory source cannot fit in cache.
    else {
        prefetch(p_source_handle + 512, simd::MM_HINT_NTA);
        /* TODO: This could be improved by using aligned-streaming when
         * possible. */
        while (bytes >= 256) {
#pragma GCC unroll 8
            for (i4 i = 0; i < 8; i++) {
                vectors[i] = meta::bit_cast<VectorType*>((p_source_handle))[i];
            }
            prefetch(p_source_handle + 512, simd::MM_HINT_NTA);
            p_source_handle += 256;
#pragma GCC unroll 8
            for (i4 i = 0; i < 8; i++) {
                stream_in(p_destination_handle, &vectors[i]);
            }
            p_destination_handle += 256;
            bytes -= 256;
        }
        simd::sfence();
    }
    std::copy_memory(p_source_handle, p_destination_handle, bytes);
    simd::zero_upper_avx_registers();
}  // namespace simd