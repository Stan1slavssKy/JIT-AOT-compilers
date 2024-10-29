cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(COMPILER)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(COMPILER_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(COMPILER_BINARY_ROOT ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra -Werror -Wshadow -Wno-invalid-offsetof")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${COMPILER_BINARY_ROOT}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${COMPILER_BINARY_ROOT}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${COMPILER_BINARY_ROOT}/bin)

set(SOURCES
    ir/graph.cpp
    ir/ir_builder.cpp
    ir/basic_block.cpp
    ir/dump_instructions.cpp
    analisys/rpo.cpp
    analisys/dfs.cpp
    analisys/dominator_tree.cpp
    analisys/loop_analizer.cpp
)

add_library(compiler_static STATIC ${SOURCES})
target_include_directories(compiler_static PUBLIC ${COMPILER_ROOT})

# --------------------------clang-format--------------------------------------

file(GLOB_RECURSE ALL_SOURCES_FILES *.cpp *.h)

find_program(CLANG_FORMAT_EXECUTABLE NAMES clang-format-14)

add_custom_target(clang-force-format
    COMMAND ${CLANG_FORMAT_EXECUTABLE}
    -i
    -style=file
    --verbose
    ${ALL_SOURCES_FILES}
)

# --------------------------google-tests--------------------------------------

enable_testing()
find_package(GTest REQUIRED)

add_executable(unit_tests)

target_link_libraries(unit_tests PUBLIC GTest::gtest_main)

add_subdirectory(tests)

# ----------------------------------------------------------------------------
