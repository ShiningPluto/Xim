//
// Created by ximu on 17-4-27.
//

#pragma once

#include "Lexer.h"

#include <string>
#include <memory>

class Lexer;

class Parser
{
    Lexer lexer;
    std::string file_path;
    std::vector<char> source_u8;
    std::vector<char32_t> source_u32;

public:
    Parser(std::string const& source);

    void run();
};