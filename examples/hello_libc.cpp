#include <cstdlib>
#include <unistd.h>

auto main() -> int {
    char8_t const* p_str = "Hello, Conscat!\n";
    if (write(1, p_str, 16) == -1) {
        exit(EXIT_FAILURE);
    };
};
