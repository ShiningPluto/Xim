//
// Created by ximu on 4/21/17.
//

#include "Token.h"

#include <unordered_map>

namespace
{
    const std::unordered_map<std::string, TokenType> keyword_map
    {
        #define KEYWORD(X,Y) {Y, TokenType::X},
        #include "Token.def"
    };

    const std::unordered_map<std::string, TokenType> punctuator_map
    {
        #define PUNCTUATOR(X,Y) {Y, TokenType::X},
        #include "Token.def"
    };

    inline std::pair<bool, TokenType> isOperator(std::unordered_map<std::string, TokenType> const& map, std::string const& s)
    {
        auto it = map.find(s);
        if (it == map.end())
            return std::make_pair(false, TokenType::Eof);
        else
            return std::make_pair(true, it->second);
    }
}

std::pair<bool, TokenType> Token::isKeyword(std::string const& s)
{
    return isOperator(keyword_map, s);
}

std::pair<bool, TokenType> Token::isPunctuator(std::string const& s)
{
    return isOperator(punctuator_map, s);
}

Token::Token(size_t l, size_t c, TokenType type_, std::string const* value_)
    : type(type_), value(value_), line(l), column(c)
{

}

std::ostream& operator<<(std::ostream& out, Token const& token)
{
    out << token.line << ':' << token.column << ' ';
    switch (token.type)
    {
        case TokenType::Identifier: {
            out << "Identifier: " << *token.value;
            break;
        }
        case TokenType::Number: {
            out << "Number: " << *token.value;
            break;
        }
        case TokenType::Eof: {
            out << "EOF";
            break;
        }
        default: {
            if (token.type > TokenType::Arrow)
                out << "Keyword: " << *token.value;
            else
                out << "Punctuator: " << *token.value;
        }
    }
    out << "\n";
    return out;
}
