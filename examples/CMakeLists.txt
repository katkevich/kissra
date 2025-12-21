cmake_minimum_required(VERSION 3.30)

add_executable(import_kissra_example
    main.cpp
)
target_compile_features(import_kissra_example PRIVATE cxx_std_26)
set_target_properties(import_kissra_example PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
    CXX_MODULE_STD ON
)
target_link_libraries(import_kissra_example PUBLIC kissra::kissra)