#include <cat/variant>

auto main() -> int {
    cat::Variant<int, char, uint4> variant(int{1});
    Result(variant.holds_alternative<int>()).or_exit();
    int foo_int = variant.get<int>();
    Result(foo_int == 1).or_exit();

    static_assert(variant.index<int> == 0u);
    static_assert(variant.index<char> == 1u);
    static_assert(variant.index<uint4> == 2u);

    variant = 'o';
    Result(variant.holds_alternative<char>()).or_exit();
    char foo_char = variant.get<char>();
    Result(foo_char == 'o').or_exit();

    cat::Optional opt1 = variant.get_if<char>();
    Result(opt1.has_value()).or_exit();
    cat::Optional opt2 = variant.get_if<int>();
    Result(!opt2.has_value()).or_exit();

    // Test variant subtype constructor and assignment operator.
    cat::Variant<int, char, uint4, int2> variant2 = variant;
    Result(variant2.holds_alternative<char>()).or_exit();
    variant2 = 1;
    Result(variant2.holds_alternative<int>()).or_exit();
    variant2 = variant;
    Result(variant2.holds_alternative<char>()).or_exit();

    variant = 1;
    cat::Variant<int, char, uint4, int2> variant3 = variant;
    Result(variant3.holds_alternative<int>()).or_exit();
    variant3 = int2{10};
    Result(variant3.holds_alternative<int2>()).or_exit();
    variant3 = variant;
    Result(variant3.holds_alternative<int>()).or_exit();

    // Test getting variant type by index.
    static_assert(cat::is_same<decltype(variant3.get<0>()), int>);
    static_assert(cat::is_same<decltype(variant3.get<1>()), char>);
    static_assert(cat::is_same<decltype(variant3.get<2>()), uint4>);
    static_assert(cat::is_same<decltype(variant3.get<3>()), int2>);
};