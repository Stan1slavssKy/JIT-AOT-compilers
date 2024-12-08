cmake_minimum_required(VERSION 3.13)

set(SOURCES 
    rpo_test.cpp
    dominator_tree_test.cpp
    loop_analyzer_test.cpp
)

add_library(analysis_tests_obj OBJECT ${SOURCES})
target_include_directories(analysis_tests_obj PUBLIC ${COMPILER_ROOT})

add_dependencies(analysis_tests_obj compiler_static)
target_link_libraries(analysis_tests_obj PUBLIC compiler_static)
