#include "benchmark/benchmark.h"
#include "kissra/fn/convert.hpp"
#include <algorithm>
#include <array>
#include <charconv>
#include <format>
#include <optional>
#include <ranges>
#include <string>
#include <vector>

#include <cmath>

using test_num_t = uint64_t;
test_num_t num = 658723948;

static void BM_IntToCharsDigits10(benchmark::State& state) {
    for (auto _ : state) {
        std::array<char, std::numeric_limits<test_num_t>::digits10 + 2> buffer;
        std::to_chars_result result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), num);
        if (result.ec == std::errc()) {
            std::string s(buffer.data(), result.ptr);
            benchmark::DoNotOptimize(s);
        }
    }
}
BENCHMARK(BM_IntToCharsDigits10);


static void BM_IntToCharsKissra(benchmark::State& state) {
    for (auto _ : state) {
        std::string s = kissra::fn::to_chars(num);
        benchmark::DoNotOptimize(s);
    }
}
BENCHMARK(BM_IntToCharsKissra);

static void BM_IntToCharsDigits(benchmark::State& state) {
    for (auto _ : state) {
        std::array<char, std::numeric_limits<test_num_t>::digits + 2> buffer;
        std::to_chars_result result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), num);
        if (result.ec == std::errc()) {
            std::string s(buffer.data(), result.ptr);
            benchmark::DoNotOptimize(s);
        }
    }
}
BENCHMARK(BM_IntToCharsDigits);

static void BM_IntFormat(benchmark::State& state) {
    for (auto _ : state) {
        std::string s = std::format("{}", num);
        benchmark::DoNotOptimize(s);
    }
}
BENCHMARK(BM_IntFormat);

static void BM_IntToString(benchmark::State& state) {
    for (auto _ : state) {
        std::string s = std::to_string(num);
        benchmark::DoNotOptimize(s);
    }
}
BENCHMARK(BM_IntToString);
