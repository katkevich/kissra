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

TEST_CASE("apply [kissra::compose().filter(...).transform(...).filter(...)] should work") {
    std::array arr = { 1, 2, 10, 3, 4 };

    auto comp = kissra::compose() //
                    .filter(fn::even)
                    .transform([](int i) { return i + 2; })
                    .filter(fn::even)
                    .filter(fn::divisible_by_c<3>);

    auto iter = kissra::all(arr).apply(comp);

    std::array expected = { 12, 6 };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        CHECK_EQ(*expected_it++, *item);
    }
}

TEST_CASE("apply [kissra::compo::filter(...).transform(...).filter(...)] should work") {
    std::array arr = { 1, 2, 10, 3, 4 };

    auto comp = kissra::compo::filter(fn::even) //
                    .transform([](int i) { return i + 2; })
                    .filter(fn::even)
                    .filter(fn::divisible_by_c<3>);

    auto iter = kissra::all(arr).apply(comp);

    std::array expected = { 12, 6 };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        CHECK_EQ(*expected_it++, *item);
    }
}

TEST_CASE("apply [kissra::compo::transform(...).filter(...)] should work") {
    std::array arr = { 1, 2, 10, 3, 4 };

    auto comp = kissra::compo::transform([](int i) { return i + 2; }) //
                    .filter(fn::even)
                    .filter(fn::divisible_by_c<3>);

    auto iter = kissra::all(arr).apply(comp);

    std::array expected = { 12, 6 };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        CHECK_EQ(*expected_it++, *item);
    }
}


} // namespace kissra::test