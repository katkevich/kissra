#include "kissra/doctest_printers.hpp"

#include "kissra/kissra.hpp"
#include <algorithm>
#include <forward_list>
#include <iostream>
#include <list>
#include <string_view>
#include <vector>

namespace kissra::test {
using std::string_view_literals::operator""sv;

TEST_CASE("all(<forward_iter>).take(N).collect() (i.e. .next()) N is less than the size") {
    std::forward_list arr = { 1, 2, 3, 4, 5 };

    auto actual = kissra::all(arr).take(2).collect();
    REQUIRE_EQ(actual, (std::vector{ 1, 2 }));
}

TEST_CASE("all(<forward_iter>).take(N).collect() (i.e. .next()) N is equal to the size") {
    std::forward_list arr = { 1, 2, 3 };

    auto actual = kissra::all(arr).take(3).collect();
    REQUIRE_EQ(actual, (std::vector{ 1, 2, 3 }));
}

TEST_CASE("all(<forward_iter>).take(N).collect() (i.e. .next()) N is greater than the size") {
    std::forward_list arr = { 1, 2, 3 };

    auto actual = kissra::all(arr).take(5).collect();
    REQUIRE_EQ(actual, (std::vector{ 1, 2, 3 }));
}

TEST_CASE("all(<forward_iter>).take(N).take(M).collect() both N and M are greater than the size (N > M)") {
    std::forward_list arr = { 1, 2, 3 };

    auto actual = kissra::all(arr).take(5).take(4).collect();
    REQUIRE_EQ(actual, (std::vector{ 1, 2, 3 }));
}


TEST_CASE("all(<forward_iter>).take(N).take(M).collect() both N and M are greater than the size (M > N)") {
    std::forward_list arr = { 1, 2, 3 };

    auto actual = kissra::all(arr).take(4).take(5).collect();
    REQUIRE_EQ(actual, (std::vector{ 1, 2, 3 }));
}

TEST_CASE("all(<forward_iter>).drop(N).take(M).collect() N + M < size") {
    std::forward_list arr = { 1, 2, 3, 4, 5, 6 };

    auto actual = kissra::all(arr).drop(2).take(3).collect();
    REQUIRE_EQ(actual, (std::vector{ 3, 4, 5 }));
}

TEST_CASE("all(<forward_iter>).drop(N).take(M).collect() N + M == size") {
    std::forward_list arr = { 1, 2, 3, 4, 5, 6 };

    auto actual = kissra::all(arr).drop(2).take(4).collect();
    REQUIRE_EQ(actual, (std::vector{ 3, 4, 5, 6 }));
}

TEST_CASE("all(<forward_iter>).drop(N).take(M).collect() N + M > size") {
    std::forward_list arr = { 1, 2, 3, 4, 5, 6 };

    auto actual = kissra::all(arr).drop(2).take(6).collect();
    REQUIRE_EQ(actual, (std::vector{ 3, 4, 5, 6 }));
}


TEST_CASE("all(<random access>).drop_last(N).take(M).collect() N + M < size") {
    std::array arr = { 1, 2, 3, 4, 5, 6 };

    auto actual = kissra::all(arr).drop_last(2).take(3).collect();
    REQUIRE_EQ(actual, (std::vector{ 1, 2, 3 }));
}

TEST_CASE("all(<random access>).drop_last(N).take(M).collect() N + M == size") {
    std::array arr = { 1, 2, 3, 4, 5, 6 };

    auto actual = kissra::all(arr).drop_last(2).take(4).collect();
    REQUIRE_EQ(actual, (std::vector{ 1, 2, 3, 4 }));
}

TEST_CASE("all(<random access>).drop_last(N).take(M).collect() N + M > size") {
    std::array arr = { 1, 2, 3, 4, 5, 6 };

    auto actual = kissra::all(arr).drop_last(2).take(6).collect();
    REQUIRE_EQ(actual, (std::vector{ 1, 2, 3, 4 }));
}

TEST_CASE("all(<random access>).drop(N).reverse().take(M).collect() N + M < size") {
    std::array arr = { 1, 2, 3, 4, 5, 6 };

    auto actual = kissra::all(arr).drop(2).reverse().take(3).collect();
    REQUIRE_EQ(actual, (std::vector{ 6, 5, 4 }));
}

TEST_CASE("all(<random access>).drop_last(N).reverse().take(M).collect() N + M < size") {
    std::array arr = { 1, 2, 3, 4, 5, 6 };

    auto actual = kissra::all(arr).drop_last(2).reverse().take(3).collect();
    REQUIRE_EQ(actual, (std::vector{ 4, 3, 2 }));
}

TEST_CASE("all(<forward_iter>).take(N) / all(arr).take(M) each taking elements from the beginning") {
    std::forward_list arr = { 1, 2, 3, 4, 5, 6 };

    auto iter = kissra::all(arr);

    {
        auto actual = iter.take(2).collect();
        REQUIRE_EQ(actual, (std::vector{ 1, 2 }));
    }
    {
        auto actual = iter.take(3).collect();
        REQUIRE_EQ(actual, (std::vector{ 1, 2, 3 }));
    }
}


TEST_CASE("all(<random access>).take(N).next_back() N is less than the size") {
    std::array arr = { 1, 2, 3, 4, 5 };

    auto iter = kissra::all(arr).take(3);

    std::array expected = { 3, 2, 1 };
    auto expected_it = expected.begin();

    while (auto item = iter.next_back()) {
        CHECK_EQ(*item, *expected_it++);
    }
}

TEST_CASE("all(<random access>).take(N).next_back() N is equal to the size") {
    std::array arr = { 1, 2, 3 };

    auto iter = kissra::all(arr).take(3);

    std::array expected = { 3, 2, 1 };
    auto expected_it = expected.begin();

    while (auto item = iter.next_back()) {
        CHECK_EQ(*item, *expected_it++);
    }
}

TEST_CASE("all(<random access>).take(N).next_back() N is greater than the size") {
    std::array arr = { 1, 2, 3 };

    auto iter = kissra::all(arr).take(5);

    std::array expected = { 3, 2, 1 };
    auto expected_it = expected.begin();

    while (auto item = iter.next_back()) {
        CHECK_EQ(*item, *expected_it++);
    }
}

TEST_CASE("all(<random access>).take(N).take(M).collect() both N and M are greater than the size (N > M)") {
    std::array arr = { 1, 2, 3 };

    auto iter = kissra::all(arr).take(5).take(4);

    std::array expected = { 3, 2, 1 };
    auto expected_it = expected.begin();

    while (auto item = iter.next_back()) {
        CHECK_EQ(*item, *expected_it++);
    }
}

TEST_CASE("all(<forward iter>).take(N).chunk(M).next() where M < N should work") {
    std::list arr = { 1, 2, 3, 4, 5, 6, 7, 8 };

    auto iter = kissra::all(arr).take(5).chunk(2);

    std::array expected = { std::vector{ 1, 2 }, std::vector{ 3, 4 }, std::vector{ 5 } };
    auto expected_it = expected.begin();

    while (auto item = iter.next()) {
        CHECK_EQ(item->collect(), *expected_it++);
    }
}

TEST_CASE("all(<random access>).take(N).chunk(M).next_back() where M < N should work") {
    std::array arr = { 1, 2, 3, 4, 5, 6, 7, 8 };

    auto iter = kissra::all(arr).take(5).chunk(2);

    std::array expected = { std::vector{ 5 }, std::vector{ 3, 4 }, std::vector{ 1, 2 } };
    auto expected_it = expected.begin();

    while (auto item = iter.next_back()) {
        CHECK_EQ(item->collect(), *expected_it++);
    }
}

} // namespace kissra::test