#include "benchmark/benchmark.h"
#include "kissra/kissra.hpp"
#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <format>
#include <optional>
#include <random>
#include <ranges>
#include <string>
#include <vector>

// ~/dev/perf-microsoft-kernel/perf-6.6.0/tools/perf/perf stat ./build/refl-release/benchmarks/kissra_bench_tests --benchmark_filter=FilterStress_HandWrittenLoop_8K

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

static void FilterStress_HandWrittenLoop_N(int N, int seed, benchmark::State& state) {
    std::vector<int> source = make_random_vector(N, seed);

    for (auto _ : state) {
        std::vector<int> result;

        for (int i : source) {
            if (i % 2 == 0)
                continue;
            if (i % 3 == 0)
                continue;
            if (i % 5 == 0)
                continue;
            if (i % 7 == 0)
                continue;
            if (i % 11 == 0)
                continue;
            // if (i % 2 != 0 && i % 3 != 0 && i % 5 != 0 && i % 7 != 0 && i % 11 != 0) {
            result.push_back(i);
            // }
        }

        benchmark::DoNotOptimize(result);
    }
}

static void FilterStress_Kissra_N(int N, int seed, benchmark::State& state) {
    std::vector<int> source = make_random_vector(N, seed);

    namespace fn = kissra::fn;

    for (auto _ : state) {
        std::vector result = kissra::all(source)
                                 .filter(fn::not_divisible_by_c<2>)
                                 .filter(fn::not_divisible_by_c<3>)
                                 .filter(fn::not_divisible_by_c<5>)
                                 .filter(fn::not_divisible_by_c<7>)
                                 .filter(fn::not_divisible_by_c<11>)
                                 .collect();

        benchmark::DoNotOptimize(result);
    }
}

static void FilterStress_StdRanges_N(int N, int seed, benchmark::State& state) {
    std::vector<int> source = make_random_vector(N, seed);

    namespace fn = kissra::fn;

    for (auto _ : state) {
        // clang-format off
        std::vector result = source
                                 | std::views::filter(fn::not_divisible_by_c<2>)
                                 | std::views::filter(fn::not_divisible_by_c<3>)
                                 | std::views::filter(fn::not_divisible_by_c<5>)
                                 | std::views::filter(fn::not_divisible_by_c<7>)
                                 | std::views::filter(fn::not_divisible_by_c<11>)
                                 | std::ranges::to<std::vector<int>>();
        // clang-format on

        benchmark::DoNotOptimize(result);
    }
}

static void FilterStress_HandWrittenLoop_8K(benchmark::State& state) {
    FilterStress_HandWrittenLoop_N(8192, 25, state);
}
static void FilterStress_Kissra_8K(benchmark::State& state) {
    FilterStress_Kissra_N(8192, 25, state);
}
static void FilterStress_StdRanges_8K(benchmark::State& state) {
    FilterStress_StdRanges_N(8192, 25, state);
}

static void FilterStress_HandWrittenLoop_4K(benchmark::State& state) {
    FilterStress_HandWrittenLoop_N(4096, 25, state);
}
static void FilterStress_Kissra_4K(benchmark::State& state) {
    FilterStress_Kissra_N(4096, 25, state);
}
static void FilterStress_StdRanges_4K(benchmark::State& state) {
    FilterStress_StdRanges_N(4096, 25, state);
}

static void FilterStress_HandWrittenLoop_2K(benchmark::State& state) {
    FilterStress_HandWrittenLoop_N(2048, 25, state);
}
static void FilterStress_Kissra_2K(benchmark::State& state) {
    FilterStress_Kissra_N(2048, 25, state);
}
static void FilterStress_StdRanges_2K(benchmark::State& state) {
    FilterStress_StdRanges_N(2048, 25, state);
}

BENCHMARK(FilterStress_HandWrittenLoop_8K);
BENCHMARK(FilterStress_Kissra_8K);
BENCHMARK(FilterStress_StdRanges_8K);

BENCHMARK(FilterStress_HandWrittenLoop_4K);
BENCHMARK(FilterStress_Kissra_4K);
BENCHMARK(FilterStress_StdRanges_4K);

BENCHMARK(FilterStress_HandWrittenLoop_2K);
BENCHMARK(FilterStress_Kissra_2K);
BENCHMARK(FilterStress_StdRanges_2K);
