#include "kissra/doctest_printers.hpp"

#include "kissra/kissra.hpp"
#include <algorithm>
#include <charconv>
#include <forward_list>
#include <iostream>
#include <list>
#include <random>
#include <vector>

namespace kissra::test {
using namespace std::string_literals;

std::vector<int> make_random_vector(int size, int seed) {
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> distr;

    std::vector<int> random_numbers;
    random_numbers.reserve(size);

    for (int i = 0; i < size; ++i) {
        random_numbers.push_back(distr(gen));
    }

    return random_numbers;
}

TEST_CASE("ensure FilterStress benchmark is correct") {
    std::vector<int> source = make_random_vector(8192, 25);

    std::vector<int> std_result;
    for (int i : source) {
        if (i % 2 != 0 && i % 3 != 0 && i % 5 != 0 && i % 7 != 0 && i % 11 != 0) {
            std_result.push_back(i);
        }
    }

    std::vector kissra_result = kissra::all(source)
                                    .filter(fn::not_divisible_by_c<2>)
                                    .filter(fn::not_divisible_by_c<3>)
                                    .filter(fn::not_divisible_by_c<5>)
                                    .filter(fn::not_divisible_by_c<7>)
                                    .filter(fn::not_divisible_by_c<11>)
                                    .collect();

    REQUIRE_EQ(std_result, kissra_result);
}

} // namespace kissra::test