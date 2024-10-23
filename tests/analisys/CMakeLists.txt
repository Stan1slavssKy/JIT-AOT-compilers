cmake_minimum_required(VERSION 3.13)

set(SOURCES rpo_test.cpp)

add_library(rpo_test_obj OBJECT ${SOURCES})
target_include_directories(rpo_test_obj PUBLIC ${COMPILER_ROOT})

add_dependencies(rpo_test_obj compiler_static)
target_link_libraries(rpo_test_obj PUBLIC compiler_static)
