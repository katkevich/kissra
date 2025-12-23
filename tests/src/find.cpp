#include "kissra/doctest_printers.hpp"

#include "kissra/kissra.hpp"
#include <algorithm>
#include <deque>
#include <forward_list>
#include <iostream>
#include <list>
#include <set>
#include <unordered_set>
#include <vector>

namespace kissra::test {
using namespace std::string_literals;

TEST_CASE("find(value) returns correct item if value exists") {
    std::array arr = { 1, 1, 2, 2, 3, 3 };
    const auto val = kissra::all(arr).find(2);

    REQUIRE_EQ(std::addressof(*val), std::addressof(arr[2]));
}

TEST_CASE("find_if(predicate) returns correct item if value exists") {
    std::array arr = { 1, 1, 2, 2, 3, 3 };
    const auto val = kissra::all(arr).find_if(kissra::fn::even);

    REQUIRE_EQ(std::addressof(*val), std::addressof(arr[2]));
}

TEST_CASE("find(value, proj) returns correct item if value exists") {
    std::array arr = { "1"s, "22"s, "333"s };
    const auto val = kissra::all(arr).find(2, kissra::fn::size);

    REQUIRE_EQ(std::addressof(*val), std::addressof(arr[1]));
}

TEST_CASE("find_not(value) returns correct item if value exists") {
    std::array arr = { 1, 1, 2, 2, 3, 3 };
    const auto val = kissra::all(arr).find_not(1);

    REQUIRE_EQ(std::addressof(*val), std::addressof(arr[2]));
}

TEST_CASE("find_if_not(predicate) returns correct item if value exists") {
    std::array arr = { 1, 1, 2, 2, 3, 3 };
    const auto val = kissra::all(arr).find_if_not(kissra::fn::odd);

    REQUIRE_EQ(std::addressof(*val), std::addressof(arr[2]));
}

TEST_CASE("find_not(value, proj) returns correct item if value exists") {
    std::array arr = { "1"s, "22"s, "333"s };
    const auto val = kissra::all(arr).find_not(1, kissra::fn::size);

    REQUIRE_EQ(std::addressof(*val), std::addressof(arr[1]));
}

TEST_CASE("find(value) returns nothing item if value doesn't exist") {
    std::array arr = { 1, 1, 2, 2, 3, 3 };
    const auto val = kissra::all(arr).find(5);

    REQUIRE_FALSE(val);
}

TEST_CASE("find_not(value) returns nothing item if all values are what is being find") {
    std::array arr = { 1, 1, 1 };
    const auto val = kissra::all(arr).find_not(1);

    REQUIRE_FALSE(val);
}

} // namespace kissra::test