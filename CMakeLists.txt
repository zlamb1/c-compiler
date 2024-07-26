cmake_minimum_required(VERSION 3.22.1)

project(c-compiler)

set(CMAKE_CXX STANDARD 11)
set(CMAKE_CXX_STANDARD_REQIRED True)

add_executable(${CMAKE_PROJECT_NAME} src/main.cpp 
    src/compiler/compiler.cpp 
    src/gen/asm/generator.cpp 
    src/ir/tac.cpp
    src/lexer/lexer.cpp 
    src/lexer/stack_str.cpp)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE src)

find_program(BASH_PROGRAM bash)

enable_testing()
set(UNIT_TEST ${CMAKE_PROJECT_NAME})

# add tests
add_subdirectory(examples)

# automatically run tests at build
# comment out to build without passing tests
# add_custom_target(run_unit_test ALL
#    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
#    DEPENDS ${UNIT_TEST})