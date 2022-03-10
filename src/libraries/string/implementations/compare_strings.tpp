// -*- mode: c++ -*-
// vim: set ft=cpp:
#pragma once

#include <buffer>
#include <limits>
#include <string>

auto std::compare_strings(StringView const& string_1,
                          StringView const& string_2) -> bool {
    // This function's design is based on
    // https://github.com/kalamay/pcmp/

    if (string_1.length != string_2.length) {
        return false;
    }

    // TODO: Support Armv8a and i386.
#ifdef __x86_64__
    using VectorType = charx32;
#endif

    Buffer<VectorType, 4> vector_1;
    Buffer<VectorType, 4> vector_2;
    Buffer<VectorType, 4> mask;
    Buffer<uint4, 4> results;
    isize length = string_1.length;
    isize vector_size = sizeof(VectorType);
    char const* p_chars_1 = string_1.p_data;
    char const* p_chars_2 = string_2.p_data;

    auto loop = [&](isize size) -> bool {
        for (; length >= vector_size * size; length -= vector_size * size,
                                             p_chars_1 += vector_size * size,
                                             p_chars_2 += vector_size * size) {
            for (int i = 0; i < size; i++) {
                vector_1[i] = *(meta::bit_cast<VectorType*>(string_1.p_data) +
                                (i * size));
                vector_2[i] = *(meta::bit_cast<VectorType*>(string_2.p_data) +
                                (i * size));
                mask[i] = vector_1[i] + vector_2[i];
                results[i] = simd::move_mask(mask[i]);
            }

            for (int i = 0; i < size; i++) {
                if (results[i] != std::numeric_limits<uint4>::max()) {
                    return false;
                }
            }
        }

        return true;
    };

    if (!loop(4)) {
        return false;
    }
    if (!loop(2)) {
        return false;
    }
    if (!loop(1)) {
        return false;
    }

    for (; length > 0; length--, p_chars_1++, p_chars_2++) {
        if (*p_chars_1 != *p_chars_2) {
            return false;
        }
    }

    return true;
}
