#include "kissra/doctest_printers.hpp"

#include "kissra/kissra.hpp"
#include <algorithm>
#include <forward_list>
#include <iostream>
#include <list>
#include <vector>

namespace kissra::test {
using namespace std::string_literals;

TEST_CASE("transform().drop(N).back() should NOT call projection more than once per item") {
    std::array arr = { 1, 2, 3, 4, 5 };

    int counter = 0;
    auto iter = kissra::all(arr)
                    .transform([&](int i) {
                        ++counter;
                        return i;
                    })
                    .drop(3);

    REQUIRE_EQ(counter, 0);

    {
        const auto item = iter.back();
        CHECK_EQ(*item, 5);
        CHECK_EQ(counter, 1);
    }
    {
        const auto item = iter.back();
        CHECK_EQ(*item, 5);
        CHECK_EQ(counter, 2);
    }
}

TEST_CASE("transform().drop_last(N).front() should NOT call projection more than once per item") {
    std::array arr = { 1, 2, 3, 4, 5 };

    int counter = 0;
    auto iter = kissra::all(arr)
                    .transform([&](int i) {
                        ++counter;
                        return i;
                    })
                    .drop_last(3);

    REQUIRE_EQ(counter, 0);

    {
        const auto item = iter.front();
        CHECK_EQ(*item, 1);
        CHECK_EQ(counter, 1);
    }
    {
        const auto item = iter.front();
        CHECK_EQ(*item, 1);
        CHECK_EQ(counter, 2);
    }
}

} // namespace kissra::test