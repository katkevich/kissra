#include <doctest/doctest.h>

#include "kissra/kissra.hpp"
#include <algorithm>
#include <forward_list>
#include <iostream>
#include <list>
#include <set>
#include <unordered_set>
#include <vector>
#include <deque>

KISSRA_REGISTER_MIXINS_BEGIN
KISSRA_REGISTER_BUILTIN_MIXINS
KISSRA_REGISTER_MIXINS_END


namespace kissra::test {
using namespace std::string_literals;

TEST_CASE("filter(<cond>).collect() should produce std::vector") {
    std::array arr = { 1, 2, 3, 4, 5 };
    REQUIRE_EQ(kissra::all(arr).filter(kissra::even).collect(), (std::vector{ 2, 4 }));
}

TEST_CASE("filter(<cond>).collect<std::set>() should produce std::set") {
    std::array arr = { 1, 2, 2, 4, 5 };
    REQUIRE_EQ(kissra::all(arr).filter(kissra::even).collect<std::set>(), (std::set{ 2, 4 }));
}

TEST_CASE("filter(<cond>).collect<std::unordered_set>() should produce std::unordered_set") {
    std::array arr = { 1, 2, 2, 4, 5 };
    REQUIRE_EQ(kissra::all(arr).filter(kissra::even).collect<std::unordered_set>(), (std::unordered_set{ 2, 4 }));
}

TEST_CASE("filter(<cond>).collect<std::list>() should produce std::list") {
    std::array arr = { 1, 2, 2, 4, 5 };
    REQUIRE_EQ(kissra::all(arr).filter(kissra::even).collect<std::list>(), (std::list{ 2, 2, 4 }));
}

TEST_CASE("filter(<cond>).collect<std::forward_list>() should produce std::forward_list") {
    std::array arr = { 1, 2, 2, 4, 5 };
    REQUIRE_EQ(kissra::all(arr).filter(kissra::even).collect<std::list>(), (std::list{ 2, 2, 4 }));
}

TEST_CASE("filter(<cond>).collect<std::deque>() should produce std::deque") {
    std::array arr = { 1, 2, 2, 4, 5 };
    REQUIRE_EQ(kissra::all(arr).filter(kissra::even).collect<std::deque>(), (std::deque{ 2, 2, 4 }));
}

} // namespace kissra::test