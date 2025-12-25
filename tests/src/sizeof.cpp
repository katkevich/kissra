#include "kissra/doctest_printers.hpp"

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

    auto iter = kissra::all(arr)                                       // 16 bytes (2 pointers)
                    .drop_last(1)                                      // random-access specialization take no space!
                    .drop(1)                                           // random-access specialization take no space!
                    .filter(fn::even)                                  // empty functor take no space!
                    .drop_while(fn::even)                              // 8 bytes (`dropped` flag)
                    .reverse()                                         // empty
                    .drop(5)                                           // 8 bytes
                    .drop(5)                                           // 8 bytes
                    .drop_while(fn::even)                              // 8 bytes (`dropped` flag)
                    .reverse()                                         // empty
                    .drop_last_while(fn::even)                         // 8 bytes (`dropped` flag)
                    .reverse()                                         // empty
                    .drop_last_while(fn::even)                         // 8 bytes (`dropped` flag)
                    .filter(fn::odd)                                   // empty functor take no space!
                    .reverse()                                         // empty
                    .drop_last(1)                                      // 8 bytes
                    .filter(fn::odd)                                   // empty functor take no space!
                    .drop_last(1)                                      // 8 bytes
                    .filter(fn::even)                                  // empty functor take no space!
                    .transform(fn::to_chars)                           // empty functor take no space!
                    .transform([](const auto& s) { return s.size(); }) // empty functor take no space!
                    .filter(fn::even)                                  // empty functor take no space!
                    .chunk(2)                                          // 8 bytes - non-random specialization
                    .chunk(2)                                          // 8 bytes - non-random specialization
                    .filter([](const auto& s) { return true; })        // empty functor take no space!
                    .zip(arr, arr)                                     // 32 bytes (4 pointers)
                    .zip(arr);                                         // 16 bytes (2 pointers)

    constexpr auto s0 = sizeof(iter);
    static_assert(sizeof(iter) == 128); //  "should" be 144 really - likely `drop_last_while::dropped` share space with its base iter thanks to [[no_unique_address of `base_iter`]])
    CHECK_LE(sizeof(iter), 144);

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