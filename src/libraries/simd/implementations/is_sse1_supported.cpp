// -*- mode: c++ -*-
// vim: set ft=cpp:
#include <simd>

// TODO: Document.
auto is_sse1_supported() -> bool1 {
    return __builtin_cpu_supports("sse");
}
