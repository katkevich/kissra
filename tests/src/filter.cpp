#include "kissra/doctest_printers.hpp"

#include "kissra/kissra.hpp"
#include <algorithm>
#include <forward_list>
#include <iostream>
#include <list>
#include <vector>

namespace kissra::test {
using namespace std::string_literals;

TEST_CASE("filter_iter::nth(0) should filter out an element but not move the cursor") {
    std::array arr = { 1, 2, 3, 4 };

    auto iter = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*iter.nth(0), 2);
}

TEST_CASE("filter_iter::front() should filter out an element but not move the cursor") {
    std::array arr = { 1, 2, 3, 4 };

    auto iter = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*iter.front(), 2);
}

TEST_CASE("filter_iter::nth(0) should not move cursor") {
    std::array arr = { 2, 3, 4 };

    auto iter = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*iter.nth(0), 2);
}

TEST_CASE("filter_iter::front() should not move cursor") {
    std::array arr = { 2, 3, 4 };

    auto iter = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*iter.front(), 2);
}

TEST_CASE("filter_iter::nth_back(0) should filter out an element but not move the sentinel") {
    std::array arr = { 1, 2, 3, 4, 5 };

    auto iter = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*iter.nth_back(0), 4);
}

TEST_CASE("filter_iter::back() should filter out an element but not move the sentinel") {
    std::array arr = { 1, 2, 3, 4, 5 };

    auto iter = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*iter.back(), 4);
}

TEST_CASE("filter_iter::nth_back(0) should not move the sentinel") {
    std::array arr = { 1, 2, 3, 4 };

    auto iter = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*iter.nth_back(0), 4);
}

TEST_CASE("filter_iter::back() should not move the sentinel") {
    std::array arr = { 1, 2, 3, 4 };

    auto iter = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*iter.nth_back(0), 4);
}


TEST_CASE("filter_iter::next() should filter out an element AND move the cursor") {
    std::array arr = { 1, 2, 3, 4 };

    auto iter = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*iter.next(), 2);
    REQUIRE_EQ(*iter.front(), 4);
}

TEST_CASE("filter_iter::next() should move the cursor") {
    std::array arr = { 2, 3, 4 };

    auto iter = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*iter.next(), 2);
    REQUIRE_EQ(*iter.front(), 4);
}

TEST_CASE("filter_iter::next_back() should filter out an element AND move the cursor") {
    std::array arr = { 1, 2, 3, 4, 5 };

    auto iter = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*iter.next_back(), 4);
    REQUIRE_EQ(*iter.back(), 2);
}

TEST_CASE("filter_iter::next_back() should move the cursor") {
    std::array arr = { 1, 2, 3, 4 };

    auto iter = kissra::all(arr).filter([](const auto& s) { return s % 2 == 0; });
    REQUIRE_EQ(*iter.next_back(), 4);
    REQUIRE_EQ(*iter.back(), 2);
}

TEST_CASE("two opposite consecutive filters filter out all elements") {
    std::array arr = { 1, 2, 3, 4 };

    {
        auto iter = kissra::all(arr) //
                        .filter([](const auto& s) { return s % 2 == 0; })
                        .filter([](const auto& s) { return s % 2 != 0; });
        REQUIRE_FALSE(iter.next());
    }
    {
        auto iter = kissra::all(arr) //
                        .filter([](const auto& s) { return s % 2 == 0; })
                        .filter([](const auto& s) { return s % 2 != 0; });
        REQUIRE_FALSE(iter.next_back());
    }
    {
        auto iter = kissra::all(arr) //
                        .filter([](const auto& s) { return s % 2 == 0; })
                        .filter([](const auto& s) { return s % 2 != 0; });
        REQUIRE_FALSE(iter.back());
    }
    {
        auto iter = kissra::all(arr) //
                        .filter([](const auto& s) { return s % 2 == 0; })
                        .filter([](const auto& s) { return s % 2 != 0; });
        REQUIRE_FALSE(iter.front());
    }
}

TEST_CASE("filter_iter predicate should be callable with destructured aggregate arg (lvalue binds to lvalue arg)") {
    struct aggregate {
        std::string s{};
        int i{};
    };
    std::array arr = {
        aggregate{ "1"s, 1 },
        aggregate{ "22"s, 2 },
    };

    auto iter = kissra::all(arr).filter([](std::string& s, int& i) { return s.size() == i; });
    REQUIRE_EQ(iter.front()->i, 1);
    REQUIRE_EQ(iter.back()->i, 2);
}

TEST_CASE("filter_iter predicate should be callable with destructured aggregate arg (prvalue binds to rvalue arg)") {
    struct aggregate {
        std::string s{};
        int i{};
    };
    std::array arr = {
        aggregate{ "1"s, 1 },
        aggregate{ "22"s, 2 },
    };

    auto iter = kissra::all(arr) //
                    .transform([](aggregate& a) { return a; })
                    .filter([](std::string&& s, int&& i) { return s.size() == i; });
    REQUIRE_EQ(iter.front()->i, 1);
    REQUIRE_EQ(iter.back()->i, 2);
}

TEST_CASE("filter_iter predicate should be callable with destructured aggregate arg (xvalue binds to rvalue arg)") {
    struct aggregate {
        std::string s{};
        int i{};
    };
    std::array arr = {
        aggregate{ "1"s, 1 },
        aggregate{ "22"s, 2 },
    };

    auto iter = kissra::all(arr) //
                    .transform([](aggregate& a) -> aggregate&& { return std::move(a); })
                    .filter([](std::string&& s, int&& i) { return s.size() == i; });
    REQUIRE_EQ(iter.front()->i, 1);
    REQUIRE_EQ(iter.back()->i, 2);
}

TEST_CASE(
    "filter_iter predicate should be callable with destructured aggregate arg (xvalue binds to const lvalue arg)") {
    struct aggregate {
        std::string s{};
        int i{};
    };
    std::array arr = {
        aggregate{ "1"s, 1 },
        aggregate{ "22"s, 2 },
    };

    auto iter = kissra::all(arr) //
                    .transform([](aggregate& a) -> aggregate&& { return std::move(a); })
                    .filter([](const std::string& s, const int& i) { return s.size() == i; });
    REQUIRE_EQ(iter.front()->i, 1);
    REQUIRE_EQ(iter.back()->i, 2);
}

TEST_CASE("filter_iter predicate should be callable with destructured tuple arg (prvalue binds to rvalue arg)") {
    struct aggregate {
        std::string s{};
        int i{};
    };
    std::array arr = {
        aggregate{ "1"s, 1 },
        aggregate{ "22"s, 2 },
    };

    auto iter = kissra::all(arr) //
                    .transform([](aggregate& a) { return std::make_tuple(a.s, a.i); })
                    .filter([](std::string&& s, int i) { return s.size() == i; });
    REQUIRE_EQ(std::get<1>(*iter.front()), 1);
}

TEST_CASE("kissra::filter(container, fn) should produce kissra::iterator") {
    std::array arr = { 1, 2, 3 };
    auto iter = kissra::filter(arr, fn::even).transform(fn::to_chars);

    REQUIRE_EQ(*iter.front(), "2"s);
}

TEST_CASE("kissra::filter(kissra::iterator, fn) should produce kissra::iterator") {
    std::array arr = { 4, 1, 2, 3 };
    auto iter = kissra::filter(kissra::all(arr).drop(1), fn::even).transform(fn::to_chars);

    REQUIRE_EQ(*iter.front(), "2"s);
}

} // namespace kissra::test