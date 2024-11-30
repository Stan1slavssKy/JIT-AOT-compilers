cmake_minimum_required(VERSION 3.13)

set(SOURCES 
    rpo_test.cpp
    dominator_tree_test.cpp
    loop_analizer_test.cpp
)

add_library(analisys_tests_obj OBJECT ${SOURCES})
target_include_directories(analisys_tests_obj PUBLIC ${COMPILER_ROOT})

add_dependencies(analisys_tests_obj compiler_static)
target_link_libraries(analisys_tests_obj PUBLIC compiler_static)
