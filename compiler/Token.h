//
// Created by ximu on 4/21/17.
//

#pragma once

enum TokenType
{
    EOF,
    Identifier,
    Keyword,
    Number,
    Punctuator;
};

enum PunctuatorType
{
    LeftSquare, // [
    RightSquare,// ]
    LeftParen, // (
    RightParen, // )
    LeftBrace, // {
    RightBrace, // }
    Period, // .
    Colon, // :
    SemiColon, // ;
    Arrow // ->
};


enum KeywordType
{
    Return,
    Def,
    Func,
    Int
};

class Token
{

};
