//
// Created by ximu on 4/20/17.
//

#pragma once

#include <string>
#include <memory>
#include <unordered_set>
#include <vector>
#include <uchar.h>

class Token;

class Lexer
{
    std::shared_ptr<std::string> file_path;
    std::unordered_set<std::string> dictionary;
    std::vector<char> source_u8;
    std::vector<char32_t> source_u32;
    std::vector<char32_t>::iterator it;

public:
    Lexer(std::shared_ptr<std::string> source);

    ~Lexer();

    Token nextToken();
};
