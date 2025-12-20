#
# cmake --build build/refl-release --target kissra_bench_tests && ./build/refl-release/benchmarks/kissra_bench_tests
#
cmake_minimum_required(VERSION 3.30)

include(FetchContent)

#
# Benchmarks (loops vs standard ranges vs kissra ranges)
#
FetchContent_Declare(benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.4
    GIT_SHALLOW 1
    EXCLUDE_FROM_ALL TRUE
)
set(BENCHMARK_ENABLE_TESTING OFF)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
set(BENCHMARK_DOWNLOAD_DEPENDENCIES ON)
FetchContent_MakeAvailable(benchmark)

add_executable(kissra_bench_tests
    # src/benchmarks.cpp
    # src/num_to_string.cpp
    # src/front_vs_nth.cpp
    src/filter_stress.cpp
)
target_compile_features(kissra_bench_tests PRIVATE cxx_std_26)
target_link_libraries(kissra_bench_tests kissra::kissra_classic benchmark::benchmark_main)