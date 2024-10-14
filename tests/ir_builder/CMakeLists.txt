cmake_minimum_required(VERSION 3.13)

set(SOURCES ir_builder.cpp)

add_library(ir_builder_test_obj OBJECT ${SOURCES})
target_include_directories(ir_builder_test_obj PUBLIC ${COMPILER_ROOT})

add_dependencies(ir_builder_test_obj compiler_static)
target_link_libraries(ir_builder_test_obj PUBLIC compiler_static)
