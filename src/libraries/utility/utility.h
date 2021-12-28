// -*- mode: c++ -*-
// vim: set ft=cpp:
#pragma once

#include <type_traits.h>

namespace meta {

template <typename T>
constexpr auto move(T&& input) -> meta::remove_reference_t<T>&&;

template <typename T>
constexpr auto forward(meta::remove_reference_t<T>& input) -> T&&;

template <typename T>
constexpr auto forward(meta::remove_reference_t<T>&& input)
    -> T&& requires(!meta::is_lvalue_reference_v<T>);

constexpr auto is_constant_evaluated() -> bool;

consteval auto constant_evaluate(auto value);

template <typename T>
auto bit_cast(auto& from_value) -> T;

}  // namespace meta

// clangd emits diagnostic false positives in this file:
#include "./impl/bit_cast.tpp"
#include "./impl/constant_evaluate.tpp"
#include "./impl/forward.tpp"
#include "./impl/is_constant_evaluated.tpp"
#include "./impl/move.tpp"
