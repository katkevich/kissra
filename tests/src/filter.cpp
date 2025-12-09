#include <doctest/doctest.h>

#include "kissra/kissra.hpp"
#include <algorithm>
#include <forward_list>
#include <iostream>
#include <list>
#include <vector>

namespace kissra::test {
using namespace std::string_literals;

TEST_CASE("filter_iter::nth(0) should filter out an element but not move the cursor") {
    std::array arr = { 1, 2, 3, 4 };

    auto iter = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*iter.nth(0), 2);
}

TEST_CASE("filter_iter::front() should filter out an element but not move the cursor") {
    std::array arr = { 1, 2, 3, 4 };

    auto iter = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*iter.front(), 2);
}

TEST_CASE("filter_iter::nth(0) should not move cursor") {
    std::array arr = { 2, 3, 4 };

    auto iter = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*iter.nth(0), 2);
}

TEST_CASE("filter_iter::front() should not move cursor") {
    std::array arr = { 2, 3, 4 };

    auto iter = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*iter.front(), 2);
}

TEST_CASE("filter_iter::nth_back(0) should filter out an element but not move the sentinel") {
    std::array arr = { 1, 2, 3, 4, 5 };

    auto iter = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*iter.nth_back(0), 4);
}

TEST_CASE("filter_iter::back() should filter out an element but not move the sentinel") {
    std::array arr = { 1, 2, 3, 4, 5 };

    auto iter = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*iter.back(), 4);
}

TEST_CASE("filter_iter::nth_back(0) should not move the sentinel") {
    std::array arr = { 1, 2, 3, 4 };

    auto iter = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*iter.nth_back(0), 4);
}

TEST_CASE("filter_iter::back() should not move the sentinel") {
    std::array arr = { 1, 2, 3, 4 };

    auto iter = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*iter.nth_back(0), 4);
}


TEST_CASE("filter_iter::next() should filter out an element AND move the cursor") {
    std::array arr = { 1, 2, 3, 4 };

    auto iter = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*iter.next(), 2);
    REQUIRE_EQ(*iter.front(), 4);
}

TEST_CASE("filter_iter::next() should move the cursor") {
    std::array arr = { 2, 3, 4 };

    auto iter = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*iter.next(), 2);
    REQUIRE_EQ(*iter.front(), 4);
}

TEST_CASE("filter_iter::next_back() should filter out an element AND move the cursor") {
    std::array arr = { 1, 2, 3, 4, 5 };

    auto iter = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*iter.next_back(), 4);
    REQUIRE_EQ(*iter.back(), 2);
}

TEST_CASE("filter_iter::next_back() should move the cursor") {
    std::array arr = { 1, 2, 3, 4 };

    auto iter = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*iter.next_back(), 4);
    REQUIRE_EQ(*iter.back(), 2);
}

TEST_CASE("two opposite consecutive filters filter out all elements") {
    std::array arr = { 1, 2, 3, 4 };

    {
        auto iter = kissra::all(arr) //
                        .filter([](const auto& s) { return s % 2 == 0; })
                        .filter([](const auto& s) { return s % 2 != 0; });
        REQUIRE_FALSE(iter.next());
    }
    {
        auto iter = kissra::all(arr) //
                        .filter([](const auto& s) { return s % 2 == 0; })
                        .filter([](const auto& s) { return s % 2 != 0; });
        REQUIRE_FALSE(iter.next_back());
    }
    {
        auto iter = kissra::all(arr) //
                        .filter([](const auto& s) { return s % 2 == 0; })
                        .filter([](const auto& s) { return s % 2 != 0; });
        REQUIRE_FALSE(iter.back());
    }
    {
        auto iter = kissra::all(arr) //
                        .filter([](const auto& s) { return s % 2 == 0; })
                        .filter([](const auto& s) { return s % 2 != 0; });
        REQUIRE_FALSE(iter.front());
    }
}
} // namespace kissra::test