cmake_minimum_required(VERSION 3.13)

set(SOURCES
    peepholes_test.cpp
    constant_folding_test.cpp
    check_elimination_test.cpp
)

add_library(peepholes_test_obj OBJECT ${SOURCES})
target_include_directories(peepholes_test_obj PUBLIC ${COMPILER_ROOT})

add_dependencies(peepholes_test_obj compiler_static)
target_link_libraries(peepholes_test_obj PUBLIC compiler_static)
