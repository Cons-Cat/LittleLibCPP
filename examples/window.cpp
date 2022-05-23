#include <cat/allocators>
#include <cat/x11>

void meow() {
    cat::PageAllocator allocator;
    // TODO: Work on this more.
    [[maybe_unused]] x11::Connection connection =
        x11::initiate_connection(allocator).or_panic(
            "Failed to create an X context!");
    cat::exit();
}
