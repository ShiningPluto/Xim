//
// Created by ximu on 4/21/17.
//

#pragma once

#include <string>
#include <ostream>

enum TokenType
{
    Eof,
    Identifier,
    Number,

    #define TOKEN(X) X,
    #include "Token.def"
};

class Token
{
public:
    static std::pair<bool, TokenType> isKeyword(std::string const& s);
    static std::pair<bool, TokenType> isPunctuator(std::string const& s);

private:
    TokenType type;
    std::string const* value;

public:
    Token(TokenType type_, std::string const* value_ = nullptr)
        : type(type_), value(value_)
    {

    }

    TokenType getType()
    {
        return type;
    }

    std::string const& getValue()
    {
        return *value;
    }

    friend std::ostream& operator<<(std::ostream& out, Token const& token);
};
