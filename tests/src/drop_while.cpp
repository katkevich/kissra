#include <doctest/doctest.h>

#include "kissra/kissra.hpp"
#include <algorithm>
#include <forward_list>
#include <iostream>
#include <list>
#include <vector>

namespace kissra::test {
using namespace std::string_literals;
namespace fn = kissra::fn;

TEST_CASE("drop_while(<comptime_match_all>).front() should return nothing") {
    std::array arr = { 1, 2, 3, 4, 5 };
    REQUIRE_FALSE(kissra::all(arr).drop_while(fn::lt_c<6>).front());
}

TEST_CASE("drop_while(<runtime_match_all>).front() should return nothing") {
    std::array arr = { 1, 2, 3, 4, 5 };
    REQUIRE_FALSE(kissra::all(arr).drop_while(fn::lt(6)).front());
}

TEST_CASE("drop_while(...) should not drop items after first non-matching element found") {
    std::array arr = { 1, 2, 3, 4, 5, 1, 2 };
    REQUIRE_EQ(kissra::all(arr).drop_while(fn::le_c<3>).back(), 2);
    REQUIRE_EQ(kissra::all(arr).drop_while(fn::le_c<3>).front(), 4);
}

TEST_CASE("drop_last_while(<comptime_match_all>).front() should return nothing") {
    std::array arr = { 1, 2, 3, 4, 5 };
    REQUIRE_FALSE(kissra::all(arr).drop_last_while(fn::lt_c<6>).front());
}

TEST_CASE("drop_last_while(<runtime_match_all>).front() should return nothing") {
    std::array arr = { 1, 2, 3, 4, 5 };
    REQUIRE_FALSE(kissra::all(arr).drop_last_while(fn::lt(6)).front());
}

TEST_CASE("drop_last_while(...) should not drop items after first non-matching element found") {
    std::array arr = { 1, 2, 3, 4, 5, 1, 2 };
    REQUIRE_EQ(kissra::all(arr).drop_last_while(fn::le_c<3>).back(), 5);
    REQUIRE_EQ(kissra::all(arr).drop_last_while(fn::le_c<3>).front(), 1);
}

TEST_CASE("drop_while(...).reverse().drop_while(...) should trim from both sides") {
    std::array arr = { 1, 2, 3, 4, 1, 1, 1, 5, 1, 2 };
    REQUIRE_EQ(kissra::all(arr).drop_while(fn::le_c<3>).reverse().drop_while(fn::le_c<3>).back(), 4);
    REQUIRE_EQ(kissra::all(arr).drop_while(fn::le_c<3>).reverse().drop_while(fn::le_c<3>).front(), 5);
}

TEST_CASE("drop_last_while(...).reverse().drop_last_while(...) should trim from both sides") {
    std::array arr = { 1, 2, 3, 4, 1, 1, 1, 5, 1, 2 };
    REQUIRE_EQ(kissra::all(arr).drop_last_while(fn::le_c<3>).reverse().drop_last_while(fn::le_c<3>).back(), 4);
    REQUIRE_EQ(kissra::all(arr).drop_last_while(fn::le_c<3>).reverse().drop_last_while(fn::le_c<3>).front(), 5);
}
} // namespace kissra::test