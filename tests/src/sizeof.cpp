#include <doctest/doctest.h>

#include "kissra/kissra.hpp"
#include <algorithm>
#include <forward_list>
#include <iostream>
#include <list>
#include <ranges>
#include <vector>

namespace kissra::test {
using std::string_literals::operator""s;

TEST_CASE("filter_iter::nth(0) should filter out an element but not move the cursor") {
    std::array arr = { 1, 2, 3, 4 };

    auto iter = kissra::all(arr) //
                    .filter(fn::even)
                    .drop_while(fn::even)
                    .reverse()
                    .drop(5)
                    .drop(5)
                    .drop_while(fn::even)
                    .reverse()
                    .drop_last_while(fn::even)
                    .reverse()
                    .drop_last_while(fn::even)
                    .filter(fn::odd)
                    .reverse()
                    .drop_last(1)
                    .filter(fn::odd)
                    .drop_last(1)
                    .filter(fn::even)
                    .transform(fn::to_chars)
                    .transform([](const auto& s) { return s.size(); })
                    .filter(fn::even)
                    .chunk(2)
                    .chunk(2)
                    .filter([](const auto& s) { return true; })
                    .zip(arr, arr)
                    .zip(arr);

    constexpr auto s0 = sizeof(iter);
    // 16 'all' + 16 'x2 drop_while' + 16 'x2 drop_last_while' + 32 'x2 drop' + 32 'x2 drop_last' +
    // 16 'x2 chunk' + 48 'x3 zip'
    static_assert(sizeof(iter) == 16 + 16 + 16 + 32 + 32 + 16 + 48); // 176
    CHECK_EQ(sizeof(iter), 176);

    // clang-format off
    auto view = arr
        | std::views::filter(fn::even)
        | std::views::drop_while(fn::even)
        | std::views::reverse
        | std::views::drop_while(fn::even)
        | std::views::reverse
        | std::views::drop_while(fn::even) // there is no drop_last
        // | std::views::drop(5)
        | std::views::reverse
        | std::views::drop_while(fn::even) // there is no drop_last
        | std::views::filter(fn::odd)
        | std::views::reverse
        | std::views::filter(fn::odd)
        | std::views::filter(fn::even)
        | std::views::transform(fn::to_chars)
        | std::views::transform([](const auto& s) { return s.size(); })
        | std::views::filter(fn::even)
        | std::views::filter([](const auto& s) { return true; });
    // clang-format on

    constexpr auto s1 = sizeof(view.begin());
    // static_assert(sizeof(view.begin()) == 368); // 16 'all' + 16 'drop_while'
}


} // namespace kissra::test