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

TEST_CASE("drop(N).back() should return nothing if N >= size") {
    std::array arr = { 1, 2, 3, 4, 5 };

    auto iter = kissra::all(arr);
    REQUIRE_FALSE(iter.drop(5).back());
}

TEST_CASE("drop_last(N).front() should return nothing if N >= size") {
    std::array arr = { 1, 2, 3, 4, 5 };

    auto iter = kissra::all(arr);
    REQUIRE_FALSE(iter.drop_last(5).front());
}

TEST_CASE("drop_last(N).drop(M).front() should return nothing if N+M >= size") {
    std::array arr = { 1, 2, 3, 4, 5 };

    auto iter = kissra::all(arr);
    REQUIRE_FALSE(iter.drop_last(2).drop(3).front());
}

TEST_CASE("drop(N).drop_last(M).front() should return nothing if N+M >= size") {
    std::array arr = { 1, 2, 3, 4, 5 };

    auto iter = kissra::all(arr);
    REQUIRE_FALSE(iter.drop(2).drop_last(3).front());
}

TEST_CASE("drop(N).drop_last(M).back() should return nothing if N+M >= size") {
    std::array arr = { 1, 2, 3, 4, 5 };

    auto iter = kissra::all(arr);
    REQUIRE_FALSE(iter.drop(2).drop_last(3).back());
}

TEST_CASE("drop_last(N).drop(M).back() should return nothing if N+M >= size") {
    std::array arr = { 1, 2, 3, 4, 5 };

    auto iter = kissra::all(arr);
    REQUIRE_FALSE(iter.drop_last(2).drop(3).back());
}

TEST_CASE("drop(N).drop(M).front() should return nothing if N+M >= size") {
    std::array arr = { 1, 2, 3, 4, 5 };

    auto iter = kissra::all(arr);
    REQUIRE_FALSE(iter.drop(2).drop(3).front());
}
TEST_CASE("drop(N).drop(M).back() should return nothing if N+M >= size") {
    std::array arr = { 1, 2, 3, 4, 5 };

    auto iter = kissra::all(arr);
    REQUIRE_FALSE(iter.drop(2).drop(3).back());
}

TEST_CASE("drop_last(N).drop_last(M).front() should return nothing if N+M >= size") {
    std::array arr = { 1, 2, 3, 4, 5 };

    auto iter = kissra::all(arr);
    REQUIRE_FALSE(iter.drop_last(2).drop_last(3).front());
}
TEST_CASE("drop_last(N).drop_last(M).back() should return nothing if N+M >= size") {
    std::array arr = { 1, 2, 3, 4, 5 };

    auto iter = kissra::all(arr);
    REQUIRE_FALSE(iter.drop_last(2).drop_last(3).back());
}
} // namespace kissra::test