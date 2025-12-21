cmake_minimum_required(VERSION 3.30)

# This specific value changes as experimental `import std` support evolves.
# See `Help/dev/experimental.rst` in the CMake source (https://cmake.org/download/) corresponding to
# your CMake build for the exact value to use.
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")

project(kissra VERSION 0.7.1)

# Get rid of `beman::optional` once `std::optional<T&>` is released
# beman::optional
include(FetchContent)
FetchContent_Declare(beman_optional
    GIT_REPOSITORY https://github.com/bemanproject/optional.git
    GIT_TAG main
    GIT_SHALLOW 1
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(beman_optional)
target_compile_features(beman_optional INTERFACE cxx_std_26)

add_library(beman_optional_mod STATIC)
target_compile_features(beman_optional_mod PRIVATE cxx_std_26)
target_sources(beman_optional_mod PUBLIC FILE_SET CXX_MODULES FILES src/kissra/beman.cppm)
target_link_libraries(beman_optional_mod PRIVATE beman_optional)
# beman::optional


set(KISSRA_HEADERS
    include/kissra/kissra.hpp
    include/kissra/type_traits.hpp
    include/kissra/concepts.hpp
    include/kissra/impl/compose.hpp
    include/kissra/impl/custom_mixins.hpp
    include/kissra/impl/export.hpp
    include/kissra/impl/into_iter.hpp
    include/kissra/impl/iter/iter_base.hpp
    include/kissra/impl/iter/members_iter.hpp
    include/kissra/impl/iter/all_iter.hpp
    include/kissra/impl/iter/chunk_iter.hpp
    include/kissra/impl/iter/drop_iter.hpp
    include/kissra/impl/iter/drop_last_iter.hpp
    include/kissra/impl/iter/drop_last_while_iter.hpp
    include/kissra/impl/iter/drop_while_iter.hpp
    include/kissra/impl/iter/filter_iter.hpp
    include/kissra/impl/iter/reverse_iter.hpp
    include/kissra/impl/iter/transform_iter.hpp
    include/kissra/impl/iter/zip_iter.hpp
    include/kissra/impl/algo/apply_mixin.hpp
    include/kissra/impl/algo/collect_mixin.hpp
    include/kissra/impl/algo/front_mixin.hpp
    include/kissra/impl/algo/back_mixin.hpp
    include/kissra/impl/algo/empty_mixin.hpp
    include/kissra/impl/algo/find_mixin.hpp
    include/kissra/impl/algo/ssize_mixin.hpp
    include/kissra/fn/cmp.hpp
    include/kissra/fn/convert.hpp
    include/kissra/fn/member.hpp
    include/kissra/fn/misc.hpp
    include/kissra/fn/num.hpp
    include/kissra/misc/functional.hpp
    include/kissra/misc/optional.hpp
    include/kissra/misc/static_string.hpp
    include/kissra/misc/type_list.hpp
    include/kissra/misc/utility.hpp
)

#
# "Classic" header-only library
#
add_library(kissra_classic INTERFACE)
add_library(kissra::kissra_classic ALIAS kissra_classic)
target_compile_features(kissra_classic INTERFACE cxx_std_26)
set_target_properties(kissra_classic PROPERTIES
    CXX_STANDARD_REQUIRED ON
)
target_link_libraries(kissra_classic INTERFACE beman_optional)
target_sources(kissra_classic INTERFACE
    FILE_SET kissra_classic_headers_set TYPE HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES ${KISSRA_HEADERS}
)

#
# "Modularized" library (`import std` should be supported)
#
add_library(kissra STATIC)
add_library(kissra::kissra ALIAS kissra)
target_compile_features(kissra PUBLIC cxx_std_26)
set_target_properties(kissra PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
    CXX_MODULE_STD ON
)
target_link_libraries(kissra PUBLIC beman_optional_mod)
target_sources(kissra PUBLIC
    FILE_SET kissra_headers_set TYPE HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES ${KISSRA_HEADERS}
)
target_sources(kissra PUBLIC
    FILE_SET kissra_cxx_modules_set TYPE CXX_MODULES
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
    FILES src/kissra/kissra.cppm
)
target_compile_definitions(kissra PRIVATE KISSRA_MODULE)



add_subdirectory(examples EXCLUDE_FROM_ALL)
add_subdirectory(benchmarks EXCLUDE_FROM_ALL)

enable_testing()
add_subdirectory(tests EXCLUDE_FROM_ALL)
