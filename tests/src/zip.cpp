#include <doctest/doctest.h>

#include "kissra/kissra.hpp"
#include <algorithm>
#include <forward_list>
#include <iostream>
#include <list>
#include <vector>

namespace kissra::test {
using std::string_literals::operator""s;

TEST_CASE("all(A).zip(B) should work (A.size() == B.size())") {
    std::array a = { 1, 2, 3 };
    std::array b = { "1"s, "2"s, "3"s };

    auto iter = kissra::all(a).zip(b);

    std::vector expected = { std::tuple{ 1, "1"s }, std::tuple{ 2, "2"s }, std::tuple{ 3, "3"s } };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        CHECK_EQ(*item, *expected_it++);
    }
}

TEST_CASE("all(A).zip(B).reverse() should work (A.size() == B.size())") {
    std::array a = { 1, 2, 3 };
    std::array b = { "1"s, "2"s, "3"s };

    auto iter = kissra::all(a).zip(b).reverse();

    std::vector expected = { std::tuple{ 3, "3"s }, std::tuple{ 2, "2"s }, std::tuple{ 1, "1"s } };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        CHECK_EQ(*item, *expected_it++);
    }
}

TEST_CASE("all(A).zip(B) should work (A.size() < B.size() - B will be truncated)") {
    std::array a = { 1, 2, 3 };
    std::array b = { "1"s, "2"s, "3"s, "4"s };

    auto iter = kissra::all(a).zip(b);

    std::vector expected = { std::tuple{ 1, "1"s }, std::tuple{ 2, "2"s }, std::tuple{ 3, "3"s } };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        CHECK_EQ(*item, *expected_it++);
    }
}


TEST_CASE("all(A).zip(B).reverse() should work (A.size() < B.size() - B will be truncated)") {
    std::array a = { 1, 2, 3 };
    std::array b = { "1"s, "2"s, "3"s, "4"s };

    auto iter = kissra::all(a).zip(b).reverse();

    std::vector expected = { std::tuple{ 3, "3"s }, std::tuple{ 2, "2"s }, std::tuple{ 1, "1"s } };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        CHECK_EQ(*item, *expected_it++);
    }
}

TEST_CASE("all(A).zip(B) should work (A.size() > B.size() - A will be truncated)") {
    std::array a = { 1, 2, 3, 4 };
    std::array b = { "1"s, "2"s, "3"s };

    auto iter = kissra::all(a).zip(b);

    std::vector expected = { std::tuple{ 1, "1"s }, std::tuple{ 2, "2"s }, std::tuple{ 3, "3"s } };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        CHECK_EQ(*item, *expected_it++);
    }
}

TEST_CASE("all(A).zip(B).reverse() should work (A.size() > B.size() - A will be truncated)") {
    std::array a = { 1, 2, 3, 4 };
    std::array b = { "1"s, "2"s, "3"s };

    auto iter = kissra::all(a).zip(b).reverse();

    std::vector expected = { std::tuple{ 3, "3"s }, std::tuple{ 2, "2"s }, std::tuple{ 1, "1"s } };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        CHECK_EQ(*item, *expected_it++);
    }
}

TEST_CASE("all(A).zip(B, C, D) should work (A.size() == B.size() == C.size() == D.size())") {
    std::array a = { 1, 2, 3 };
    std::array b = { "1"s, "2"s, "3"s };
    std::array c = { 4, 5, 6 };
    std::array d = { "44"s, "55"s, "66"s };

    auto iter = kissra::all(a).zip(b, c, d);

    std::vector expected = {
        std::tuple{ 1, "1"s, 4, "44"s },
        std::tuple{ 2, "2"s, 5, "55"s },
        std::tuple{ 3, "3"s, 6, "66"s },
    };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        CHECK_EQ(*item, *expected_it++);
    }
}

TEST_CASE("all(A).zip(B, C, D).reverse() should work (A.size() == B.size() == C.size() == D.size())") {
    std::array a = { 1, 2, 3 };
    std::array b = { "1"s, "2"s, "3"s };
    std::array c = { 4, 5, 6 };
    std::array d = { "44"s, "55"s, "66"s };

    auto iter = kissra::all(a).zip(b, c, d).reverse();

    std::vector expected = {
        std::tuple{ 3, "3"s, 6, "66"s },
        std::tuple{ 2, "2"s, 5, "55"s },
        std::tuple{ 1, "1"s, 4, "44"s },
    };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        CHECK_EQ(*item, *expected_it++);
    }
}

TEST_CASE("all(A).zip(B, C, D) should work (A.size() < B.size() == C.size() > D.size())") {
    std::array a = { 1, 2, 3 };
    std::array b = { "1"s, "2"s, "3"s, "4"s };
    std::array c = { 4, 5, 6, 7, 8 };
    std::array d = { "44"s, "55"s, "66"s };

    // auto it = kissra::all(a);
    auto iter = kissra::all(a).zip(b, c, d);

    std::vector expected = {
        std::tuple{ 1, "1"s, 4, "44"s },
        std::tuple{ 2, "2"s, 5, "55"s },
        std::tuple{ 3, "3"s, 6, "66"s },
    };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        CHECK_EQ(*item, *expected_it++);
    }
}

TEST_CASE("all(A).zip(B, C, D).reverse() should work (A.size() < B.size() == C.size() > D.size())") {
    std::array a = { 1, 2, 3 };
    std::array b = { "1"s, "2"s, "3"s, "4"s };
    std::array c = { 4, 5, 6, 7, 8 };
    std::array d = { "44"s, "55"s, "66"s };

    // auto it = kissra::all(a);
    auto iter = kissra::all(a).zip(b, c, d).reverse();

    std::vector expected = {
        std::tuple{ 3, "3"s, 6, "66"s },
        std::tuple{ 2, "2"s, 5, "55"s },
        std::tuple{ 1, "1"s, 4, "44"s },
    };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        CHECK_EQ(*item, *expected_it++);
    }
}

TEST_CASE("all(A).zip(lvalue_producing, rvalue_producing) should return correct value categories as a result)") {
    namespace kss = kissra;
    std::array a = { 1, 2, 3 };
    std::array b = { 4, 5, 6, 7, 8 };

    auto iter = kss::all(a).zip(b, kss::all(b).transform(fn::to_chars));

    std::vector expected = {
        std::tuple{ 1, 4, "4"s },
        std::tuple{ 2, 5, "5"s },
        std::tuple{ 3, 6, "6"s },
    };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        static_assert(std::is_same_v<decltype(item), kissra::optional<std::tuple<int&, int&, std::string>>>);

        CHECK_EQ(*item, *expected_it++);
    }
}

TEST_CASE("all(A).zip(lvalue_producing, rvalue_producing).reverse() should return correct value categories as a result)") {
    namespace kss = kissra;
    std::array a = { 1, 2, 3 };
    std::array b = { 4, 5, 6, 7, 8 };

    auto iter = kss::all(a).zip(b, kss::all(b).transform(fn::to_chars)).reverse();

    std::vector expected = {
        std::tuple{ 3, 6, "6"s },
        std::tuple{ 2, 5, "5"s },
        std::tuple{ 1, 4, "4"s },
    };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        static_assert(std::is_same_v<decltype(item), kissra::optional<std::tuple<int&, int&, std::string>>>);

        CHECK_EQ(*item, *expected_it++);
    }
}


} // namespace kissra::test