cmake_minimum_required(VERSION 3.30)

include(FetchContent)

FetchContent_Declare(beman_optional
    GIT_REPOSITORY https://github.com/bemanproject/optional.git
    GIT_TAG main
    GIT_SHALLOW 1
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(beman_optional)
target_compile_features(beman_optional INTERFACE cxx_std_26)

project(kissra VERSION 0.7.1)

set(CMAKE_CXX_EXTENSIONS OFF)
set(MORFO_HEADERS
    include/kissra/kissra.hpp
    include/kissra/type_traits.hpp
    include/kissra/concepts.hpp
    include/kissra/impl/compose.hpp
    include/kissra/impl/custom_mixins.hpp
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

add_library(kissra INTERFACE)
add_library(kissra::kissra ALIAS kissra)
target_compile_features(kissra INTERFACE cxx_std_26)
target_link_libraries(kissra INTERFACE beman_optional)


target_sources(kissra INTERFACE
    FILE_SET kissra_headers_set TYPE HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES ${MORFO_HEADERS}
)

add_subdirectory(benchmarks EXCLUDE_FROM_ALL)

enable_testing()
add_subdirectory(tests EXCLUDE_FROM_ALL)
