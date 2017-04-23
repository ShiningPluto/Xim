//
// Created by ximu on 4/21/17.
//

#pragma once

#include <string>

enum TokenType
{
    Eof,
    Identifier,
    Number,

    // Punctuator
    LeftSquare, // [
    RightSquare,// ]
    LeftParen, // (
    RightParen, // )
    LeftBrace, // {
    RightBrace, // }
    Period, // .
    Colon, // :
    SemiColon, // ;
    Arrow, // ->

    // Keyword
    Def,
    Func,
    Int,
    Return
};

class Token
{
public:
    static std::pair<bool, TokenType> isKeyword(std::string const& s);

private:
    TokenType type;
    std::string const* value;

public:
    Token(TokenType type_, std::string const* value_ = nullptr)
        : type(type_), value(value_)
    {

    }

};
