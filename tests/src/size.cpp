#include <doctest/doctest.h>

#include "kissra/kissra.hpp"
#include <algorithm>
#include <deque>
#include <format>
#include <forward_list>
#include <iostream>
#include <list>
#include <set>
#include <unordered_set>
#include <vector>

#include <iostream>

KISSRA_REGISTER_MIXINS_BEGIN
KISSRA_REGISTER_BUILTIN_MIXINS
KISSRA_REGISTER_MIXINS_END

namespace kissra::test {
using namespace std::string_literals;

TEST_CASE("all().size() should return unsigned size") {
    std::array arr = { 1, 2, 3, 4, 5 };
    REQUIRE_EQ(kissra::all(arr).size(), 5uz);
}

TEST_CASE("all().ssize() should return signed size") {
    std::array arr = { 1, 2, 3, 4, 5 };
    REQUIRE_EQ(kissra::all(arr).ssize(), 5z);
}

TEST_CASE("all().drop(N).size() should return unsigned size (N is less then size") {
    std::array arr = { 1, 2, 3, 4, 5 };
    REQUIRE_EQ(kissra::all(arr).drop(3).size(), 2uz);
}

TEST_CASE("all().drop(N).size() should return unsigned size (N is equal to size") {
    std::array arr = { 1, 2, 3, 4, 5 };
    REQUIRE_EQ(kissra::all(arr).drop(5).size(), 0uz);
}

TEST_CASE("all().drop(N).size() should return unsigned size (N is greater then size") {
    std::array arr = { 1, 2, 3, 4, 5 };
    REQUIRE_EQ(kissra::all(arr).drop(6).size(), 0uz);
}

TEST_CASE("all().drop(N).ssize() should return signed size (N is less then size") {
    std::array arr = { 1, 2, 3, 4, 5 };
    REQUIRE_EQ(kissra::all(arr).drop(3).size(), 2z);
}

TEST_CASE("all().drop_last(N).size() should return unsigned size (N is less then size") {
    std::array arr = { 1, 2, 3, 4, 5 };
    REQUIRE_EQ(kissra::all(arr).drop_last(3).size(), 2uz);
}

TEST_CASE("all().drop_last(N).size() should return unsigned size (N is equal to size") {
    std::array arr = { 1, 2, 3, 4, 5 };
    REQUIRE_EQ(kissra::all(arr).drop_last(5).size(), 0uz);
}

TEST_CASE("all().drop_last(N).size() should return unsigned size (N is greater then size") {
    std::array arr = { 1, 2, 3, 4, 5 };
    REQUIRE_EQ(kissra::all(arr).drop_last(6).size(), 0uz);
}

TEST_CASE("all().drop_last(N).size() should return signed size (N is less then size") {
    std::array arr = { 1, 2, 3, 4, 5 };
    REQUIRE_EQ(kissra::all(arr).drop_last(3).ssize(), 2z);
}

TEST_CASE("all().reverse().size() should return unsigned size") {
    std::array arr = { 1, 2, 3, 4, 5 };
    REQUIRE_EQ(kissra::all(arr).reverse().size(), 5uz);
}

TEST_CASE("all().reverse().ssize() should return signed size") {
    std::array arr = { 1, 2, 3, 4, 5 };
    REQUIRE_EQ(kissra::all(arr).reverse().ssize(), 5z);
}

TEST_CASE("all().transform(...).size() should return unsigned size") {
    std::array arr = { 1, 2, 3, 4, 5 };
    REQUIRE_EQ(kissra::all(arr).transform(kissra::fn::format_c<"{}">).ssize(), 5z);
    REQUIRE_EQ(
        kissra::all(arr).transform(kissra::fn::format_c<"{}">).collect(), (std::vector{ "1"s, "2"s, "3"s, "4"s, "5"s }));
}
} // namespace kissra::test