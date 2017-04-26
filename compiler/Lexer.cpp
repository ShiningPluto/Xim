//
// Created by ximu on 4/20/17.
//

#include "Lexer.h"

#include "Token.h"

#include <sstream>
#include <locale>
#include <cctype>
#include <fstream>

void Lexer::setSource(std::vector<char32_t> const& source)
{
    this->source = &source;
    it = source.begin();
}

Token Lexer::nextToken()
{
    if (it == source->end())
    {
        return Token(TokenType::Eof);
    }
    else
    {
        while(it!=source->end() && *it != '_' && !isalpha(*it))
        {
            ++it;
        }

        auto it2 = it;
        if (*it2 == '_' || isalpha(*it2))
        {
            do
            {
                ++it2;
            } while(it2!=source->end() && (*it2 == '_' || isalpha(*it2) || isdigit(*it2)));

            std::vector<char> buffer(size_t(it2-it)+1, char());

            std::locale locale;
            using facet_type = std::codecvt<char32_t, char, std::mbstate_t>;
            const facet_type& facet = std::use_facet<facet_type>(locale);
            std::mbstate_t state{};
            const char32_t* from_next;
            char* to_next;
            facet.out(state, it.base(), it2.base(), from_next, buffer.data(), buffer.data()+buffer.size(), to_next);
            it = it2;

            auto item = dictionary.insert(std::string(buffer.data())).first;
            auto type = Token::isKeyword(*item);
            if (type.first)
                return Token(type.second, &(*item));
            else
                return Token(TokenType::Identifier, &(*item));
        }
        else
        {
            return Token(TokenType::Eof);
        }
    }
}

