#include <doctest/doctest.h>

#include "kissra/kissra.hpp"
#include "kissra/noisy.hpp"
#include <algorithm>
#include <charconv>
#include <forward_list>
#include <iostream>
#include <list>
#include <map>
#include <type_traits>
#include <vector>

namespace kissra::test {
using std::string_literals::operator""s;

TEST_CASE("kissra::all(<aggregates>).values() should return an iterator over 1th member of an aggregate") {
    struct aggregate {
        int i{};
        std::string s;
    };
    std::array arr = { aggregate{ 1, "1"s }, aggregate{ 2, "22"s } };

    auto iter = kissra::all(arr).values();
    static_assert(std::is_same_v<typename decltype(iter)::reference, std::string&>);

    std::vector expected = { "1"s, "22"s };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        CHECK_EQ(*item, *expected_it++);
    }
}

TEST_CASE("kissra::all(<tuple>).values() should return an iterator over 0th member of a tuple") {
    std::array arr = { std::tuple{ 1, "1"s }, std::tuple{ 2, "22"s } };

    auto iter = kissra::all(arr).values();
    static_assert(std::is_same_v<typename decltype(iter)::reference, std::string&>);

    std::vector expected = { "1"s, "22"s };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        CHECK_EQ(*item, *expected_it++);
    }
}

TEST_CASE("kissra::values(<tuple or rvalue references>) propagate its value category") {
    std::string s = "1";
    std::array arr = { std::tuple<int, std::string&&>{ 1, std::move(s) } };

    auto iter = kissra::values(arr);
    static_assert(std::is_same_v<typename decltype(iter)::reference, std::string&&>);

    std::vector expected = { s };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        CHECK_EQ(*item, *expected_it++);
    }
}

TEST_CASE("kissra::all(<tuple or rvalue references>).values() propagate its value category") {
    std::string s = "1";
    std::array arr = { std::tuple<int, std::string&&>{ 1, std::move(s) } };

    auto iter = kissra::all(arr).values();
    static_assert(std::is_same_v<typename decltype(iter)::reference, std::string&&>);

    std::vector expected = { s };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        CHECK_EQ(*item, *expected_it++);
    }
}

TEST_CASE("apply [kissra::compo::values().reverse()] should work") {
    std::string s1 = "1";
    std::string s2 = "22";
    std::array arr = {
        std::tuple<int, std::string&&>{ 1, std::move(s1) },
        std::tuple<int, std::string&&>{ 22, std::move(s2) },
    };

    auto compo = kissra::compo::values().reverse();
    auto iter = kissra::all(arr).apply(compo);

    std::vector expected = { s2, s1 };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        CHECK_EQ(*item, *expected_it++);
    }
}

TEST_CASE("apply [kissra::compo::values().reverse()] to std::map should work") {
    std::map<int, std::string> m = {
        { 1, "1"s },
        { 22, "22"s },
    };

    auto compo = kissra::compo::values().reverse();
    auto iter = kissra::all(m).apply(compo);

    std::vector expected = { "22"s, "1"s };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        CHECK_EQ(*item, *expected_it++);
    }
}

TEST_CASE("kissra::all(<std::map>).values().filter(odd) should work") {
    std::map<std::string, int> m = {
        { "1"s, 1 },
        { "22"s, 22 },
        { "13"s, 13 },
    };

    auto iter = kissra::all(m).values().filter(fn::odd).reverse();

    std::vector expected = { 13, 1 };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        CHECK_EQ(*item, *expected_it++);
    }
}
} // namespace kissra::test