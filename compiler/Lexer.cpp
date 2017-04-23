//
// Created by ximu on 4/20/17.
//

#include "Lexer.h"

#include "Token.h"

#include <sstream>

#include <cctype>

Lexer::Lexer(std::shared_ptr<std::string> source)
    : file_path(source)
{
    input.open(*file_path);
}

Lexer::~Lexer()
{
    input.close();
}

Token Lexer::nextToken()
{
    if (input.eof())
    {
        return Token(TokenType::Eof);
    }
    else
    {
        int c = input.peek();
        if (c == '_' || isalpha(c))
        {
            std::ostringstream s;
            while(c=='_' || isalpha(c) || isdigit(c))
            {
                s << input.get();
                c = input.peek();
            }
            auto it = dictionary.insert(s.str()).first;
            auto it2 = Token::isKeyword(*it);
            if (it2.first)
                return Token(it2.second);
            else
                return Token(TokenType::Identifier, &(*it));
        }
        else if (isdigit(c))
        {

        }
    }
}

