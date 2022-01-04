#include <string>

void meow() {
    char8_t const* p_string = u8"Hello!";
    i4 scalar_len = string_length_as<i4>(p_string);
    i4 vector_len = simd::string_length_as<i4>(p_string);
    Result(scalar_len == vector_len).or_print_panic();
    exit(0);
}
