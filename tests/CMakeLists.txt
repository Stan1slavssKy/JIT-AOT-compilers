cmake_minimum_required(VERSION 3.13)

add_library(unit_test_main OBJECT main.cpp)

target_link_libraries(unit_tests PUBLIC
    unit_test_main
    ir_builder_test_obj
    analysis_tests_obj
    peepholes_test_obj
)

add_custom_target(run_unit_tests
    COMMENT "Running unit tests"
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/unit_tests --gtest_color=yes
)
add_dependencies(run_unit_tests unit_tests)

add_subdirectory(ir_builder)
add_subdirectory(analysis)
add_subdirectory(optimizations)
