cmake_minimum_required(VERSION 3.15)
project(out/bracketLang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(bracketLang main.cpp
        types.hpp
        lexer.cpp
        lexer.hpp
        parser.cpp
        parser.hpp
        evaluator.cpp
        evaluator.hpp
)
