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

TEST_CASE("transform(to_chars).collect() should produce dec vector<string>") {
    std::array arr = { 10, 11, 16, 17 };
    REQUIRE_EQ(kissra::all(arr).transform(kissra::fn::to_chars).collect(), (std::vector{ "10"s, "11"s, "16"s, "17"s }));
}

TEST_CASE("transform(to_chars_bin).collect() should produce bin vector<string>") {
    std::array arr = { 10, 11, 16, 17 };
    REQUIRE_EQ(kissra::all(arr).transform(kissra::fn::to_chars_bin).collect(),
        (std::vector{ "1010"s, "1011"s, "10000"s, "10001"s }));
}

TEST_CASE("transform(to_chars_oct).collect() should produce oct vector<string>") {
    std::array arr = { 10, 11, 16, 17 };
    REQUIRE_EQ(
        kissra::all(arr).transform(kissra::fn::to_chars_oct).collect(), (std::vector{ "12"s, "13"s, "20"s, "21"s }));
}

TEST_CASE("transform(to_chars_hex).collect() should produce hex vector<string>") {
    std::array arr = { 10, 11, 16, 17 };
    REQUIRE_EQ(
        kissra::all(arr).transform(kissra::fn::to_chars_hex).collect(), (std::vector{ "a"s, "b"s, "10"s, "11"s }));
}

TEST_CASE("to_chars(uint64_t::max) should work") {
    const auto v = std::numeric_limits<uint64_t>::max();
    REQUIRE_EQ(kissra::fn::to_chars(v), "18446744073709551615"s);
}

TEST_CASE("to_chars(uint64_t::min) should work") {
    const auto v = std::numeric_limits<int64_t>::min();
    REQUIRE_EQ(kissra::fn::to_chars(v), "-9223372036854775808"s);
}

TEST_CASE("to_chars_bin(uint64_t::max) should work") {
    const auto v = std::numeric_limits<uint64_t>::max();
    REQUIRE_EQ(kissra::fn::to_chars_bin(v), "1111111111111111111111111111111111111111111111111111111111111111"s);
}

TEST_CASE("to_chars_bin(uint64_t::min) should work") {
    const auto v = std::numeric_limits<int64_t>::min();
    REQUIRE_EQ(kissra::fn::to_chars_bin(v), "-1000000000000000000000000000000000000000000000000000000000000000");
}

TEST_CASE("to_chars_hex(uint64_t::max) should work") {
    const auto v = std::numeric_limits<uint64_t>::max();
    REQUIRE_EQ(kissra::fn::to_chars_hex(v), "ffffffffffffffff"s);
}

TEST_CASE("to_chars_hex(uint64_t::min) should work") {
    const auto v = std::numeric_limits<int64_t>::min();
    REQUIRE_EQ(kissra::fn::to_chars_hex(v), "-8000000000000000");
}

} // namespace kissra::test