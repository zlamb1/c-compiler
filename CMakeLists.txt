cmake_minimum_required(VERSION 3.22.1)

project(lang-lexer)

set(CMAKE_CXX STANDARD 11)
set(CMAKE_CXX_STANDARD_REQIRED True)

add_executable(${CMAKE_PROJECT_NAME} src/main.cpp src/lexer/lexer.cpp src/lexer/stack_str.cpp)