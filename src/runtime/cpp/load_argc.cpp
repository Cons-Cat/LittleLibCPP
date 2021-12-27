// -*- mode: c++ -*-
// vim: set ft=cpp:
#include <runtime.h>

auto load_argc() -> i4 {
    i4 argc;
    asm("mov 0(%%rbp), %[argc]" : [argc] "=r"(argc));
    return argc;
}
