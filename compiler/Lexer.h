//
// Created by ximu on 4/20/17.
//

#pragma once

#include "Token.h"

#include <string>
#include <memory>
#include <unordered_set>
#include <vector>
#include <queue>
#include <uchar.h>

class Lexer
{
    std::unordered_set<std::string> dictionary;
    std::vector<char32_t> const* source;
    std::vector<char32_t>::const_iterator it;
    size_t line;
    size_t column;
    std::queue<Token> buffer;

public:
    Lexer();

    ~Lexer() = default;

    void setSource(std::vector<char32_t> const& source);

    /// get next token
    /// if <buffer> is not empty, pop out the front of <buffer>
    /// else return the next token of source string
    Token nextToken();

    /// return the next token of source string and push it to
    /// the back of <buffer>
    Token peek();

    void eatToken(TokenType type);

private:
    Token consumeSource();
};
