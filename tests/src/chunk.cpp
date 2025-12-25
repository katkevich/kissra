#include "kissra/doctest_printers.hpp"

#include "kissra/kissra.hpp"
#include <algorithm>
#include <charconv>
#include <forward_list>
#include <iostream>
#include <list>
#include <vector>

namespace kissra::test {
using namespace std::string_literals;

TEST_CASE("all().chunk(2) forward iteration indeed produces chunks (no tail remainder)") {
    std::array arr = { 1, 2, 3, 4, 5, 6 };
    auto arr_it = arr.begin();

    auto iter = kissra::all(arr).chunk(2);

    while (auto chunk = iter.next()) {
        while (auto item = chunk->next()) {
            CHECK_EQ(*item, *arr_it++);
        }
    }
}

TEST_CASE("all().chunk(2) forward iteration indeed produces chunks (with tail remainder)") {
    std::array arr = { 1, 2, 3, 4, 5, 6, 7 };
    auto arr_it = arr.begin();

    auto iter = kissra::all(arr).chunk(2);

    while (auto chunk = iter.next()) {
        while (auto item = chunk->next()) {
            CHECK_EQ(*item, *arr_it++);
        }
    }
}

TEST_CASE("all().drop_while(...).chunk(2) works (drop_while pred is called only once)") {
    std::array arr = { 1, 1, 3, 4, 5, 6, 7, 8 };
    auto arr_it = arr.begin();
    arr_it += 3; // skip odd items

    int counter = 0;
    auto iter = kissra::all(arr)
                    .drop_while([&](auto x) {
                        counter++;
                        return x % 2 == 1;
                    })
                    .chunk(2);

    while (auto chunk = iter.next()) {
        while (auto item = chunk->next()) {
            CHECK_EQ(*item, *arr_it++);
        }
    }

    REQUIRE_EQ(counter, 4); // 3 skipped + 1 "first success"
}


TEST_CASE("all().drop_XXX(...)... .chunk() combos works") {
    std::array arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };

    int counter = 0;
    auto iter = kissra::all(arr) //
                    .drop(2)
                    .drop_last_while(fn::odd)
                    .drop_last_while(fn::odd)
                    .drop_while(fn::odd)
                    .drop_while(fn::odd)
                    .drop_last(2)
                    .filter(fn::even)
                    .chunk(2);

    std::vector<int> actual;
    while (auto chunk = iter.next()) {
        while (auto item = chunk->next()) {
            actual.push_back(*item);
        }
    }

    REQUIRE_EQ(actual, (std::vector{ 4, 6, 8, 10 }));
}

TEST_CASE("all().chunk(N).drop().drop_last() works") {
    std::array arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };

    int counter = 0;
    auto iter = kissra::all(arr) //
                    .chunk(2)
                    .drop(2)
                    .drop_last(2)
                    .drop_last(1);

    std::vector<int> actual;
    while (auto chunk = iter.next()) {
        while (auto item = chunk->next()) {
            actual.push_back(*item);
        }
    }

    REQUIRE_EQ(actual, (std::vector{ 5, 6, 7, 8 }));
}

TEST_CASE("all().chunk(N).drop(N).drop_last(M).nth(<within range>) should work") {
    std::array arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };

    int counter = 0;
    auto iter = kissra::all(arr) //
                    .chunk(2)
                    .drop(2)
                    .drop_last(2);

    {
        auto chunk = iter.nth(1);
        REQUIRE(chunk);

        REQUIRE_EQ(chunk->collect(), (std::vector{ 7, 8 }));
    }
    {
        auto chunk = iter.nth(2);
        REQUIRE_FALSE(chunk);
    }
}

TEST_CASE("all().chunk(N).nth(<short chunk idx>) should return short chunk") {
    std::array arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };

    int counter = 0;
    auto iter = kissra::all(arr).chunk(4);

    {
        auto chunk = iter.nth(3);
        REQUIRE(chunk);

        REQUIRE_EQ(chunk->collect(), (std::vector{ 13, 14 }));
    }
}

TEST_CASE("all().chunk(N).advance_back(M) shoudl indeed move the sentinel") {
    std::array arr = { 1, 2, 3, 4, 5, 6, 7 };

    int counter = 0;
    auto iter = kissra::all(arr).chunk(2);

    iter.advance_back(1);

    REQUIRE_EQ(iter.back()->collect(), (std::vector{ 5, 6 }));
}

TEST_CASE("all().chunk(N).nth_back(0) should return short chunk") {
    std::array arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };

    int counter = 0;
    auto iter = kissra::all(arr).chunk(4);

    {
        auto chunk = iter.nth_back(0);
        REQUIRE(chunk);

        REQUIRE_EQ(chunk->collect(), (std::vector{ 13, 14 }));
    }
}

TEST_CASE("all().chunk(N).back() should return short chunk") {
    std::array arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };

    int counter = 0;
    auto iter = kissra::all(arr).chunk(4);

    {
        auto chunk = iter.back();
        REQUIRE(chunk);

        REQUIRE_EQ(chunk->collect(), (std::vector{ 13, 14 }));
    }
}

TEST_CASE("all().chunk(N).drop(N).size() should return correct size (handle short chunk)") {
    std::array arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };

    int counter = 0;
    auto iter = kissra::all(arr).chunk(4).drop(1);

    REQUIRE_EQ(iter.size(), 3);
}

TEST_CASE("all().filter(...).chunk(2) works (filter pred is called while evaluating the end of each chunk") {
    std::array arr = { 0, 1, 2, 3, 4, 5, 6, 7 };

    int counter = 0;
    auto iter = kissra::all(arr)
                    .filter([&](auto x) {
                        counter++;
                        return fn::odd(x);
                    })
                    .chunk(2);

    while (auto chunk = iter.next()) {
    }

    /**
     * ff - fast-forward items
     * [0] chunk: [1 (FF "0") + 1 (FF "1")] + [1 ("1") + 1 ("2") + 1 ("3") + 1 ("4") + 1 ("5")]
     * [1] chunk: [1 (FF "5")] + [1 ("5") + 1 ("6") + 1 ("7")]
     */
    REQUIRE_EQ(counter, 11);
}

TEST_CASE("all().filter(...).chunk(2) works (filter pred is called while evaluating the end of each chunk") {
    std::array arr = { 0, 0, 1, 2, 3, 4, 5, 6, 7 };

    int counter = 0;
    auto iter = kissra::all(arr)
                    .filter([&](auto x) {
                        counter++;
                        return fn::odd(x);
                    })
                    .chunk(2);

    std::vector<int> actual;
    while (auto chunk = iter.next()) {
        while (auto item = chunk->next()) {
            actual.push_back(*item);
        }
    }

    /**
     * ff - fast-forward items
     * [0] chunk: [1 (FF "0") + 1 (FF "0") + 1 (FF "1")] + [1 ("1") + 1 ("2") + 1 ("3") + 1 ("4") + 1 ("5")]
     * [1] chunk: [1 (FF "5")] + [1 ("5") + 1 ("6") + 1 ("7")]
     *
     * chunk[0]: 5: "1", "2", "3"
     * chunk[1]: 3: "4", "5", "6", "7"
     */
    REQUIRE_EQ(counter, 12 + 7);
    REQUIRE_EQ(actual, (std::vector{ 1, 3, 5, 7 }));
}

TEST_CASE("all().reverse().chunk(2) should produce chunks backwards") {
    std::array arr = { 0, 1, 2, 3, 4, 5, 6 };

    auto iter = kissra::all(arr).reverse().chunk(2);
    auto expected = std::vector{ std::vector{ 6, 5 }, std::vector{ 4, 3 }, std::vector{ 2, 1 }, std::vector{ 0 } };
    auto expected_it = expected.begin();

    while (auto ch = iter.next()) {
        CHECK_EQ(ch->collect(), *expected_it++);
    }
}

TEST_CASE("all().chunk(2).chunk(2) should work") {
    std::array arr = { 0, 1, 2, 3, 4, 5, 6 };

    auto iter = kissra::all(arr).chunk(2).chunk(2);

    auto expected = std::vector{
        std::vector{ std::vector{ 0, 1 }, std::vector{ 2, 3 } },
        std::vector{ std::vector{ 4, 5 }, std::vector{ 6 } },
    };
    auto expected_outer_it = expected.begin();

    while (auto outer_chunk = iter.next()) {
        auto expected_inner_it = (expected_outer_it++)->begin();
        while (auto inner_chunk = outer_chunk->next()) {
            CHECK_EQ(inner_chunk->collect(), *expected_inner_it++);
        }
    }
}

TEST_CASE("all().zip(...).chunk(2).next() should work") {
    std::array arr = { 0, 1, 2, 3 };
    std::list lst = { 0.f, 1.f, 2.f };

    auto iter = kissra::zip(arr, lst).chunk(2);

    auto expected = std::vector{
        std::vector{ std::tuple{ 0, 0.f }, std::tuple{ 1, 1.f } },
        std::vector{ std::tuple{ 2, 2.f } },
    };
    auto expected_outer_it = expected.begin();

    while (auto chunk = iter.next()) {
        auto expected_inner_it = (expected_outer_it++)->begin();
        while (auto tuple = chunk->next()) {
            CHECK_EQ(*tuple, *expected_inner_it++);
        }
    }
}

// TODO: try to fix 'is_sized' (it should work here ideally, but it will be tricky to make it work) & test
TEST_CASE("all().zip(...).chunk(2).next_back() should work") {
    std::array arr = { 0, 1, 2, 3 };
    std::array lst = { 0.f, 1.f, 2.f };

    auto iter = kissra::zip(arr, lst).chunk(2);

    auto expected = std::vector{
        std::vector{ std::tuple{ 2, 2.f } },
        std::vector{ std::tuple{ 0, 0.f }, std::tuple{ 1, 1.f } },
    };
    auto expected_outer_it = expected.begin();

    while (auto chunk = iter.next_back()) {
        auto expected_inner_it = (expected_outer_it++)->begin();
        while (auto tuple = chunk->next()) {
            CHECK_EQ(*tuple, *expected_inner_it++);
        }
    }
}

// TODO: ...and here (or not? we can't make chunk sized from std::list, coz chunk can be short and we need "random-access" to determine its size)
TEST_CASE("all().zip(...).chunk(2).next_back() should work (iterate chunk in reverse)") {
    std::array arr = { 0, 1, 2, 3 };
    std::array lst = { 0.f, 1.f, 2.f };

    auto iter = kissra::zip(arr, lst).chunk(2);

    auto expected = std::vector{
        std::vector{ std::tuple{ 1, 1.f }, std::tuple{ 0, 0.f } },
        std::vector{ std::tuple{ 2, 2.f } },
    };
    auto expected_outer_it = expected.begin();

    while (auto chunk = iter.next()) {
        auto expected_inner_it = (expected_outer_it++)->begin();
        while (auto tuple = chunk->next_back()) {
            CHECK_EQ(*tuple, *expected_inner_it++);
        }
    }
}


TEST_CASE("all().zip(...).reverse().chunk(2).next() should work (zipped array has same length)") {
    std::array arr = { 0, 1, 2 };
    std::array lst = { 0.f, 1.f, 2.f };

    auto iter = kissra::zip(arr, lst).reverse().chunk(2);

    auto expected = std::vector{
        std::vector{ std::tuple{ 2, 2.f }, std::tuple{ 1, 1.f } },
        std::vector{ std::tuple{ 0, 0.f } },
    };
    auto expected_outer_it = expected.begin();

    while (auto chunk = iter.next()) {
        auto expected_inner_it = (expected_outer_it++)->begin();
        while (auto tuple = chunk->next()) {
            CHECK_EQ(*tuple, *expected_inner_it++);
        }
    }
}

TEST_CASE("all().zip(...).reverse().chunk(2).next() should work (zipped array has different length)") {
    std::array arr = { 0, 1, 2, 3, 4 };
    std::array lst = { 0.f, 1.f, 2.f };

    auto iter = kissra::zip(arr, lst).reverse().chunk(2);

    auto expected = std::vector{
        std::vector{ std::tuple{ 2, 2.f }, std::tuple{ 1, 1.f } },
        std::vector{ std::tuple{ 0, 0.f } },
    };
    auto expected_outer_it = expected.begin();

    while (auto chunk = iter.next()) {
        auto expected_inner_it = (expected_outer_it++)->begin();
        while (auto tuple = chunk->next()) {
            CHECK_EQ(*tuple, *expected_inner_it++);
        }
    }
}
} // namespace kissra::test