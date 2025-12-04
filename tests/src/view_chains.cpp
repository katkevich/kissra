#include <doctest/doctest.h>

#include "kissra/kissra.hpp"
#include <algorithm>
#include <charconv>
#include <forward_list>
#include <iostream>
#include <list>
#include <vector>

KISSRA_REGISTER_MIXINS_BEGIN
KISSRA_REGISTER_BUILTIN_MIXINS
KISSRA_REGISTER_MIXINS_END


namespace kissra::test {
using namespace std::string_literals;

TEST_CASE("filter + transform + transform + filter") {
    std::array arr = { "1"s, "22"s, "333"s, "4444"s, "55555"s, "666666"s };

    auto view = kissra::all(arr)
                    .filter([](const auto& s) { return s.size() != 2; })
                    .transform([](const auto& s) { return std::atoi(s.c_str()); })
                    .transform([](int i) { return i + 3; })
                    .filter([](int i) { return i % 2 == 0; });

    std::vector<int> actual;
    while (auto item = view.next()) {
        actual.push_back(*item);
    }
    std::vector expected = { 4, 336, 55558 };

    REQUIRE_EQ(actual, expected);
}

TEST_CASE("filter + transform + reverse + transform + filter") {
    std::array arr = { "1"s, "22"s, "333"s, "4444"s, "55555"s, "666666"s };

    auto view = kissra::all(arr)
                    .filter([](const auto& s) { return s.size() != 2; })
                    .transform([](const auto& s) { return std::atoi(s.c_str()); })
                    .reverse()
                    .transform([](int i) { return i + 3; })
                    .filter([](int i) { return i % 2 == 0; });

    std::vector<int> actual;
    while (auto item = view.next()) {
        actual.push_back(*item);
    }
    std::vector expected = { 55558, 336, 4 };

    REQUIRE_EQ(actual, expected);
}

TEST_CASE("transform + filter + drop (without 'advance' shortcut coz 'filter' isn't random access)") {
    std::array arr = { "1"s, "22"s, "333"s, "4444"s, "55555"s, "666666"s, "0"s };

    auto view = kissra::all(arr)
                    .transform([](const auto& s) { return std::atoi(s.c_str()); })
                    .filter([](int i) { return i % 2 == 0; })
                    .drop(2);

    std::vector<int> actual;
    while (auto item = view.next()) {
        actual.push_back(*item);
    }
    std::vector expected = { 666666, 0 };

    REQUIRE_EQ(actual, expected);
}

TEST_CASE("transform + drop (with 'advance' shortcut coz 'transform' is random access)") {
    std::array arr = { "1"s, "22"s, "333"s, "4444"s, "55555"s, "666666"s, "0"s };

    auto view = kissra::all(arr) //
                    .transform([](const auto& s) { return std::atoi(s.c_str()); })
                    .drop(4);

    std::vector<int> actual;
    while (auto item = view.next()) {
        actual.push_back(*item);
    }
    std::vector expected = { 55555, 666666, 0 };

    REQUIRE_EQ(actual, expected);
}

TEST_CASE("transform + drop (dropping the amount equal to the size of the original collection)") {
    std::array arr = { "1"s, "22"s, "333"s };

    auto view = kissra::all(arr) //
                    .transform([](const auto& s) { return std::atoi(s.c_str()); })
                    .drop(3);

    std::vector<int> actual;
    while (auto item = view.next()) {
        actual.push_back(*item);
    }
    std::vector<int> expected = {};

    REQUIRE_EQ(actual, expected);
}

TEST_CASE("transform + drop (dropping the amount greater than the size of the original collection)") {
    std::array arr = { "1"s, "22"s, "333"s };

    auto view = kissra::all(arr) //
                    .transform([](const auto& s) { return std::atoi(s.c_str()); })
                    .drop(4);

    std::vector<int> actual;
    while (auto item = view.next()) {
        actual.push_back(*item);
    }
    std::vector<int> expected = {};

    REQUIRE_EQ(actual, expected);
}

TEST_CASE("drop + reverse (should drop from the head)") {
    std::array arr = { 1, 2, 3, 4 };

    auto view = kissra::all(arr).drop(2).reverse();

    std::vector<int> actual;
    while (auto item = view.next()) {
        actual.push_back(*item);
    }
    std::vector expected = { 4, 3 };

    REQUIRE_EQ(actual, expected);
}

TEST_CASE("drop + reverse + drop (should drop from the head (2 items) and from the tail (1 item))") {
    std::array arr = { 1, 2, 3, 4, 5, 6 };

    auto view = kissra::all(arr).drop(2).reverse().drop(1);

    std::vector<int> actual;
    while (auto item = view.next()) {
        actual.push_back(*item);
    }
    std::vector expected = { 5, 4, 3 };

    REQUIRE_EQ(actual, expected);
}

TEST_CASE("drop_last + reverse + drop_last (should drop from the head (2 items) and from the tail (1 item))") {
    std::array arr = { 1, 2, 3, 4, 5, 6 };

    auto view = kissra::all(arr).drop_last(1).reverse().drop_last(2);

    std::vector<int> actual;
    while (auto item = view.next()) {
        actual.push_back(*item);
    }
    std::vector expected = { 5, 4, 3 };

    REQUIRE_EQ(actual, expected);
}

TEST_CASE("drop + drop_last (drop and drop_last intersects)") {
    std::array arr = { 1, 2, 3, 4, 5, 6 };

    auto view = kissra::all(arr).drop(4).reverse().drop_last(4);

    std::vector<int> actual;
    while (auto item = view.next()) {
        actual.push_back(*item);
    }
    std::vector<int> expected = {};

    REQUIRE_EQ(actual, expected);
}

TEST_CASE("drop + drop_last (drop and drop_last exactly match)") {
    std::array arr = { 1, 2, 3, 4, 5, 6 };

    auto view = kissra::all(arr).drop(3).drop_last(3);

    std::vector<int> actual;
    while (auto item = view.next()) {
        actual.push_back(*item);
    }
    std::vector<int> expected = {};

    REQUIRE_EQ(actual, expected);
}

TEST_CASE("drop + drop_last (random access source collection, drop and drop_last keep exactly one element)") {
    std::array arr = { 1, 2, 3, 4, 5, 6 };

    auto view = kissra::all(arr).drop(2).drop_last(3);

    std::vector<int> actual;
    while (auto item = view.next()) {
        actual.push_back(*item);
    }
    std::vector<int> expected = { 3 };

    REQUIRE_EQ(actual, expected);
}

TEST_CASE("drop + drop_last (bidir source collection, drop and drop_last keep exactly one element)") {
    std::list coll = { 1, 2, 3, 4, 5, 6 };

    auto view = kissra::all(coll).drop(2).drop_last(3);

    std::vector<int> actual;
    while (auto item = view.next()) {
        actual.push_back(*item);
    }
    std::vector<int> expected = { 3 };

    REQUIRE_EQ(actual, expected);
}
} // namespace kissra::test