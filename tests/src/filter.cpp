#include <doctest/doctest.h>

#include "kissra/kissra.hpp"
#include <algorithm>
#include <forward_list>
#include <iostream>
#include <list>
#include <vector>

KISSRA_REGISTER_MIXINS_BEGIN
KISSRA_REGISTER_BUILTIN_MIXINS
KISSRA_REGISTER_MIXINS_END


namespace kissra::test {
using namespace std::string_literals;

TEST_CASE("filter_view::advance(0) should filter out an element but not move the cursor") {
    std::array arr = { 1, 2, 3, 4 };

    auto view = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*view.advance(0), 2);
}

TEST_CASE("filter_view::front() should filter out an element but not move the cursor") {
    std::array arr = { 1, 2, 3, 4 };

    auto view = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*view.front(), 2);
}

TEST_CASE("filter_view::advance(0) should not move cursor") {
    std::array arr = { 2, 3, 4 };

    auto view = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*view.advance(0), 2);
}

TEST_CASE("filter_view::front() should not move cursor") {
    std::array arr = { 2, 3, 4 };

    auto view = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*view.front(), 2);
}

TEST_CASE("filter_view::advance_back(0) should filter out an element but not move the sentinel") {
    std::array arr = { 1, 2, 3, 4, 5 };

    auto view = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*view.advance_back(0), 4);
}

TEST_CASE("filter_view::back() should filter out an element but not move the sentinel") {
    std::array arr = { 1, 2, 3, 4, 5 };

    auto view = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*view.back(), 4);
}

TEST_CASE("filter_view::advance_back(0) should not move the sentinel") {
    std::array arr = { 1, 2, 3, 4 };

    auto view = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*view.advance_back(0), 4);
}

TEST_CASE("filter_view::back() should not move the sentinel") {
    std::array arr = { 1, 2, 3, 4 };

    auto view = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*view.advance_back(0), 4);
}


TEST_CASE("filter_view::next() should filter out an element AND move the cursor") {
    std::array arr = { 1, 2, 3, 4 };

    auto view = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*view.next(), 2);
    REQUIRE_EQ(*view.front(), 4);
}

TEST_CASE("filter_view::next() should move the cursor") {
    std::array arr = { 2, 3, 4 };

    auto view = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*view.next(), 2);
    REQUIRE_EQ(*view.front(), 4);
}

TEST_CASE("filter_view::next_back() should filter out an element AND move the cursor") {
    std::array arr = { 1, 2, 3, 4, 5 };

    auto view = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*view.next_back(), 4);
    REQUIRE_EQ(*view.back(), 2);
}

TEST_CASE("filter_view::next_back() should move the cursor") {
    std::array arr = { 1, 2, 3, 4 };

    auto view = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*view.next_back(), 4);
    REQUIRE_EQ(*view.back(), 2);
}

TEST_CASE("two opposite consecutive filters filter out all elements") {
    std::array arr = { 1, 2, 3, 4 };

    {
        auto view = kissra::all(arr) //
                        .filter([](const auto& s) { return s % 2 == 0; })
                        .filter([](const auto& s) { return s % 2 != 0; });
        REQUIRE_FALSE(view.next());
    }
    {
        auto view = kissra::all(arr) //
                        .filter([](const auto& s) { return s % 2 == 0; })
                        .filter([](const auto& s) { return s % 2 != 0; });
        REQUIRE_FALSE(view.next_back());
    }
    {
        auto view = kissra::all(arr) //
                        .filter([](const auto& s) { return s % 2 == 0; })
                        .filter([](const auto& s) { return s % 2 != 0; });
        REQUIRE_FALSE(view.back());
    }
    {
        auto view = kissra::all(arr) //
                        .filter([](const auto& s) { return s % 2 == 0; })
                        .filter([](const auto& s) { return s % 2 != 0; });
        REQUIRE_FALSE(view.front());
    }
}
} // namespace kissra::test