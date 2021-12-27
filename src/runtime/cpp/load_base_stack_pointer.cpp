// -*- mode: c++ -*-
// vim: set ft=cpp:
#include <runtime.h>

auto load_base_stack_pointer() -> void* {
    void* rbp;
    asm("mov %%rbp, %[rbp]" : [rbp] "=r"(rbp));
    return rbp;
}