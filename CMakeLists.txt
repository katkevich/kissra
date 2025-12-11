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

project(kissra VERSION 0.7.0)

set(CMAKE_CXX_EXTENSIONS OFF)
set(MORFO_HEADERS
    include/kissra/kissra.hpp
    include/kissra/type_traits.hpp
    include/kissra/concepts.hpp
    include/kissra/registered_mixins_fwd.hpp
    include/kissra/iter/iter_base.hpp
    include/kissra/iter/all_iter.hpp
    include/kissra/iter/chunk_iter.hpp
    include/kissra/iter/drop_iter.hpp
    include/kissra/iter/drop_last_iter.hpp
    include/kissra/iter/drop_last_while_iter.hpp
    include/kissra/iter/drop_while_iter.hpp
    include/kissra/iter/filter_iter.hpp
    include/kissra/iter/reverse_iter.hpp
    include/kissra/iter/transform_iter.hpp
    include/kissra/iter/zip_iter.hpp
    include/kissra/algo/collect_mixin.hpp
    include/kissra/algo/size_mixin.hpp
    include/kissra/algo/front_mixin.hpp
    include/kissra/algo/back_mixin.hpp
    include/kissra/algo/empty_mixin.hpp
    include/kissra/algo/find_mixin.hpp
    include/kissra/algo/ssize_mixin.hpp
    include/kissra/fn/cmp.hpp
    include/kissra/fn/num.hpp
    include/kissra/fn/convert.hpp
    include/kissra/fn/misc.hpp
    include/kissra/misc/fwd.hpp
    include/kissra/misc/into_iter.hpp
    include/kissra/misc/optional.hpp
    include/kissra/misc/static_string.hpp
    include/kissra/misc/type_list.hpp
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
