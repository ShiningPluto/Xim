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

    Punctuator,
    #define PUNCTUATOR(X, Y) X,
    #include "Token.def"

    Keyword,
    #define KEYWORD(X, Y) X,
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
    size_t line;
    size_t column;

public:
    Token() = default;

    Token(size_t l, size_t c, TokenType type_ = TokenType::Eof, std::string const* value_ = nullptr);

    TokenType getType()
    {
        return type;
    }

    void set(TokenType type_, std::string const* value_ = nullptr)
    {
        type = type_;
        value = value_;
    }

    std::string const& getValue()
    {
        return *value;
    }

    friend std::ostream& operator<<(std::ostream& out, Token const& token);
};
