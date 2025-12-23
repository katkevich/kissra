#include "kissra/doctest_printers.hpp"

#include "kissra/kissra.hpp"
#include <algorithm>
#include <charconv>
#include <forward_list>
#include <iostream>
#include <list>
#include <vector>

namespace kissra::test {
using namespace std::string_literals;

TEST_CASE("transform().filter(cond).filter(opposite_cond) is empty") {
    std::array arr = { "1"s, "22"s, "333"s, "4444"s, "55555"s, "666666"s };

    auto iter = kissra::all(arr) //
                    .transform([](const auto& s) { return std::atoi(s.c_str()); })
                    .filter(fn::even)
                    .filter(fn::odd);

    REQUIRE(iter.empty());
}

TEST_CASE("filter(cond).filter(opposite_cond) is empty") {
    std::array arr = { 1, 2, 3, 4, 5, 6, 7, 8 };

    auto iter = kissra::all(arr) //
                    .filter(fn::even)
                    .filter(fn::odd);

    REQUIRE(iter.empty());
}

TEST_CASE("filter(cond) is NOT empty") {
    std::array arr = { 1, 2, 3, 4, 5, 6, 7, 8 };

    auto iter = kissra::all(arr).filter(fn::even);

    REQUIRE_FALSE(iter.empty());
}

TEST_CASE("drop(1).filter(cond) is empty") {
    std::array arr = { 1, 2, 2 };

    auto iter = kissra::all(arr).drop(1).filter(fn::odd);

    REQUIRE(iter.empty());
}

} // namespace kissra::test