#include "kissra/doctest_printers.hpp"

#include "kissra/kissra.hpp"
#include "kissra/noisy.hpp"
#include <algorithm>
#include <charconv>
#include <forward_list>
#include <iostream>
#include <list>
#include <type_traits>
#include <vector>

namespace kissra::test {
using std::string_literals::operator""s;

TEST_CASE("kissra::all(<aggregates>).keys() should return an iterator over 0th member of an aggregate") {
    struct aggregate {
        std::string s;
        int i{};
    };
    std::array arr = { aggregate{ "1"s, 1 }, aggregate{ "22"s, 2 } };

    auto iter = kissra::all(arr).keys();
    static_assert(std::is_same_v<typename decltype(iter)::reference, std::string&>);

    std::vector expected = { "1"s, "22"s };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        CHECK_EQ(*item, *expected_it++);
    }
}

TEST_CASE("kissra::all(<tuple>).keys() should return an iterator over 0th member of a tuple") {
    std::array arr = { std::tuple{ "1"s, 1 }, std::tuple{ "22"s, 2 } };

    auto iter = kissra::all(arr).keys();
    static_assert(std::is_same_v<typename decltype(iter)::reference, std::string&>);

    std::vector expected = { "1"s, "22"s };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        CHECK_EQ(*item, *expected_it++);
    }
}

TEST_CASE("kissra::keys(<tuple or rvalue references>) propagate its value category") {
    std::string s = "1";
    std::array arr = { std::tuple<std::string&&>{ std::move(s) } };

    auto iter = kissra::keys(arr);
    static_assert(std::is_same_v<typename decltype(iter)::reference, std::string&&>);

    std::vector expected = { s };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        CHECK_EQ(*item, *expected_it++);
    }
}

TEST_CASE("kissra::all(<tuple or rvalue references>).keys() propagate its value category") {
    std::string s = "1";
    std::array arr = { std::tuple<std::string&&>{ std::move(s) } };

    auto iter = kissra::all(arr).keys();
    static_assert(std::is_same_v<typename decltype(iter)::reference, std::string&&>);

    std::vector expected = { s };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        CHECK_EQ(*item, *expected_it++);
    }
}

TEST_CASE("apply [kissra::compo::keys().reverse()] should work") {
    std::string s1 = "1";
    std::string s2 = "22";
    std::array arr = {
        std::tuple<std::string&&>{ std::move(s1) },
        std::tuple<std::string&&>{ std::move(s2) },
    };

    auto compo = kissra::compo::keys().reverse();
    auto iter = kissra::all(arr).apply(compo);

    std::vector expected = { s2, s1 };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        CHECK_EQ(*item, *expected_it++);
    }
}
} // namespace kissra::test