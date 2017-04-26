//
// Created by ximu on 4/21/17.
//

#include "Token.h"

#include <unordered_map>

namespace
{
    const std::unordered_map<std::string, TokenType> keyword_map
    {
        {"def",    TokenType::Def},
        {"func",   TokenType::Func},
        {"int",    TokenType::Int},
        {"return", TokenType::Return}
    };
}

std::pair<bool, TokenType> Token::isKeyword(std::string const& s)
{
    auto it = keyword_map.find(s);
    if (it == keyword_map.end())
        return std::make_pair(false, TokenType::Eof);
    else
        return std::make_pair(true, it->second);
}
