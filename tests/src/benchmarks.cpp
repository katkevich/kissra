#include "benchmark/benchmark.h"
#include "kissra/kissra.hpp"
#include <algorithm>
#include <array>
#include <optional>
#include <ranges>
#include <string>
#include <vector>

KISSRA_REGISTER_MIXINS_BEGIN
KISSRA_REGISTER_BUILTIN_MIXINS
KISSRA_REGISTER_MIXINS_END

using namespace std::string_literals;

static void BM_RawLoops(benchmark::State& state) {
    std::array arr = { "1"s, "22"s, "333"s, "4444"s, "55555"s, "666666"s };

    for (auto _ : state) {
        std::vector<int> vec;
        for (const auto& str : arr) {
            if (str.size() != 2) {

                int value = std::atoi(str.c_str());

                value += 3;

                if (value % 2 == 0) {
                    vec.push_back(value);
                }
            }
        }
    }
}
BENCHMARK(BM_RawLoops);

static void BM_KissraRangesNext(benchmark::State& state) {
    std::array arr = { "1"s, "22"s, "333"s, "4444"s, "55555"s, "666666"s };

    for (auto _ : state) {
        std::vector<int> vec;

        auto view = kissra::all(arr)
                        .filter([](const auto& s) { return s.size() != 2; })
                        .transform([](const auto& s) { return std::atoi(s.c_str()); })
                        .transform([](int i) { return i + 3; })
                        .filter([](int i) { return i % 2 == 0; });

        while (auto item = view.next()) {
            vec.push_back(*item);
        }
    }
}
BENCHMARK(BM_KissraRangesNext);

static void BM_StandardRanges(benchmark::State& state) {
    std::array arr = { "1"s, "22"s, "333"s, "4444"s, "55555"s, "666666"s };

    for (auto _ : state) {
        std::vector<int> vec;

        // clang-format off
        auto view = arr
            | std::views::filter([](const auto& s) { return s.size() != 2; })
            | std::views::transform([](const auto& s) { return std::atoi(s.c_str()); })
            | std::views::transform([](int i) { return i + 3; })
            | std::views::filter([](int i) { return i % 2 == 0; });
        // clang-format on

        for (auto&& item : view) {
            vec.push_back(item);
        }
    }
}
BENCHMARK(BM_StandardRanges);