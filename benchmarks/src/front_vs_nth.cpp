#include "benchmark/benchmark.h"
#include "kissra/kissra.hpp"
#include <algorithm>
#include <array>
#include <charconv>
#include <format>
#include <optional>
#include <ranges>
#include <string>
#include <vector>

#include <cmath>

using std::string_literals::operator""s;

static void BM_Front(benchmark::State& state) {
    std::array arr = {
        "1"s,
        "22"s,
        "333"s,
        "4444"s,
        "55555"s,
        "666666"s,
        "7777777"s,
        "88888888"s,
        "0"s,
        "xxxxxxxxxxxxxx"s,
        "ada"s,
    };

    for (auto _ : state) {
        // clang-format off
        auto item = kissra::all(arr)
            .filter([](const auto& s){ return s.size() % 6 != 0; })
            .drop_while([](const auto& s){ return s.size() < 3; })
            .drop(1)
            .drop_last(1)
            .drop_last_while([](const auto& s){ return s.size() > 9; })
            .transform([](const auto& s) { return s.size(); })
            .drop_last(1)
            .filter(kissra::fn::even)
            .reverse()
            .front();
        // clang-format on

        // item === 8
        benchmark::DoNotOptimize(item);
    }
}
BENCHMARK(BM_Front);

static void BM_AdvanceZero(benchmark::State& state) {
    std::array arr = {
        "1"s,
        "22"s,
        "333"s,
        "4444"s,
        "55555"s,
        "666666"s,
        "7777777"s,
        "88888888"s,
        "0"s,
        "xxxxxxxxxxxxxx"s,
        "ada"s,
    };

    for (auto _ : state) {
        // clang-format off
        auto item = kissra::all(arr)
            .filter([](const auto& s){ return s.size() % 6 != 0; })
            .drop_while([](const auto& s){ return s.size() < 3; })
            .drop(1)
            .drop_last(1)
            .drop_last_while([](const auto& s){ return s.size() > 9; })
            .transform([](const auto& s) { return s.size(); })
            .drop_last(1)
            .filter(kissra::fn::even)
            .reverse()
            .nth(0);
        // clang-format on

        // item === 8
        benchmark::DoNotOptimize(item);
    }
}
BENCHMARK(BM_AdvanceZero);
