cmake_minimum_required(VERSION 3.30)

include(FetchContent)

FetchContent_Declare(doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.12
    GIT_SHALLOW 1
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(doctest)

add_executable(kissra_tests
    src/iter_chains.cpp
    src/filter.cpp
    src/drop.cpp
    src/drop_while.cpp
)
target_compile_features(kissra_tests PRIVATE cxx_std_26)

target_link_libraries(kissra_tests kissra::kissra doctest::doctest_with_main)
target_compile_definitions(kissra_tests PUBLIC
    DOCTEST_CONFIG_VOID_CAST_EXPRESSIONS
)

#
# Make tests discoverable by CTest
#
list(APPEND CMAKE_MODULE_PATH ${doctest_SOURCE_DIR}/scripts/cmake)
include(doctest)
doctest_discover_tests(kissra_tests)


#
# Benchmarks (loops vs standard ranges vs kissra ranges (coroutines))
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
    src/benchmarks.cpp
)
target_compile_features(kissra_bench_tests PRIVATE cxx_std_26)
target_link_libraries(kissra_bench_tests kissra::kissra benchmark::benchmark_main)