cmake_minimum_required(VERSION 3.13)

set(SOURCES dfs_test.cpp)

add_library(dfs_test_obj OBJECT ${SOURCES})
target_include_directories(dfs_test_obj PUBLIC ${COMPILER_ROOT})

add_dependencies(dfs_test_obj compiler_static)
target_link_libraries(dfs_test_obj PUBLIC compiler_static)
