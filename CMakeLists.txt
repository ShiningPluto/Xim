cmake_minimum_required(VERSION 3.6)
project(Xim)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        compiler/compiler.cpp
        compiler/Lexer.h compiler/Lexer.cpp
        compiler/Token.cpp compiler/Token.h
        compiler/Parser.cpp compiler/Parser.h)

add_executable(compiler ${SOURCE_FILES})