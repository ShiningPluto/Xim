cmake_minimum_required(VERSION 3.6)
project(Xim)

set(CMAKE_CXX_STANDARD 11)

add_compile_options(-fPIC -fvisibility-inlines-hidden -Wall -W -Wno-unused-parameter -Wwrite-strings -Wcast-qual
        -Wno-missing-field-initializers -pedantic -Wno-long-long -Wdelete-non-virtual-dtor -Wno-comment
        -ffunction-sections -fdata-sections -g
        -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS)

set(SOURCE_FILES
        compiler/compiler.cpp
        compiler/Lexer.h compiler/Lexer.cpp
        compiler/Token.cpp compiler/Token.h
        compiler/Parser.cpp compiler/Parser.h
        compiler/AST.h compiler/AST.cpp)

add_executable(compiler ${SOURCE_FILES})

target_link_libraries(compiler -L/usr/lib/llvm-3.8/lib
        -lLLVMCore -lLLVMSupport
        -ldl -lpthread -ltinfo)