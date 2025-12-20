#
# cmake --build build/refl-release --target kissra_tests && ctest --test-dir ./build/refl-release/tests --output-on-failure
#

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
    src/benchmark.cpp
    src/chunk.cpp
    src/collect.cpp
    src/compose.cpp
    src/convert.cpp
    src/custom_mixin.cpp
    src/drop_while.cpp
    src/drop.cpp
    src/empty.cpp
    src/filter.cpp
    src/find.cpp
    src/functional.cpp
    src/iter_chains.cpp
    src/member.cpp
    src/members.cpp
    src/size.cpp
    src/transform.cpp
    src/zip.cpp
    src/sizeof.cpp
)
target_compile_features(kissra_tests PRIVATE cxx_std_26)
target_link_libraries(kissra_tests kissra::kissra_classic doctest::doctest_with_main)
target_compile_definitions(kissra_tests PUBLIC
    DOCTEST_CONFIG_VOID_CAST_EXPRESSIONS
)
target_sources(kissra_tests PRIVATE
    FILE_SET kissra_tests_headers_set TYPE HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
        include/kissra/noisy.hpp
)

#
# Make tests discoverable by CTest
#
list(APPEND CMAKE_MODULE_PATH ${doctest_SOURCE_DIR}/scripts/cmake)
include(doctest)
doctest_discover_tests(kissra_tests)
