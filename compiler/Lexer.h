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
    std::unordered_set<std::string> dictionary;
    std::vector<char32_t> const* source;
    std::vector<char32_t>::const_iterator it;
    size_t line;
    size_t column;

public:
    Lexer();

    ~Lexer() = default;

    void setSource(std::vector<char32_t> const& source);

    Token nextToken();
};
