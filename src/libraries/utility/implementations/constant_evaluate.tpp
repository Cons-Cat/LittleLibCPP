// -*- mode: c++ -*-
// vim: set ft=cpp:
#pragma once
#include <utility.h>

// TODO: add a `meta::invocable` concept.
consteval auto meta::constant_evaluate(auto value) {
    return value();
}