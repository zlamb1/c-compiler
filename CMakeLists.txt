cmake_minimum_required(VERSION 3.22.1)

project(c-compiler)

set(CMAKE_CXX STANDARD 11)
set(CMAKE_CXX_STANDARD_REQIRED True)

add_executable(${CMAKE_PROJECT_NAME} src/main.cpp src/compiler/compiler.cpp src/gen/asm/generator.cpp src/lexer/lexer.cpp src/lexer/stack_str.cpp)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE src)