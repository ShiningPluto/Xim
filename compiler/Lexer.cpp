#include "Lexer.h"

#include "Token.h"

#include <locale>
#include <fstream>
#include <iostream>

namespace
{
    // the ispunct in standard library can't handle char32_t correctly
    inline bool isPunctuationCharacter(char32_t const& c)
    {
        return (0x21<=c && c<=0x2F) // 	!"#$%&'()*+,-./
               || (0x3A<=c && c<=0x40) // :;<=>?@
            || (0x5B<=c && c<=0x60) // [\]^_`
            || (0x7B<=c && c<=0x7E); // {|}~
    }

    using const_iterator = std::vector<char32_t>::const_iterator;
    std::string toString(const_iterator const& begin, const_iterator const& end)
    {
        static std::locale locale{};
        using facet_type = std::codecvt<char32_t, char, std::mbstate_t>;
        static const facet_type& facet = std::use_facet<facet_type>(locale);

        std::vector<char> buffer(size_t(end-begin)*4+1, char());
        std::mbstate_t state{};
        const char32_t* from_next;
        char* to_next;
        facet.out(state, begin.base(), end.base(), from_next, buffer.data(), buffer.data()+buffer.size(), to_next);
        return std::string(buffer.data());
    }
}

void Lexer::setSource(std::vector<char32_t> const& source)
{
    this->source = &source;
    it = source.begin();
}

Token Lexer::nextToken()
{
    // can't handle now
    while(it != source->end() && isspace(*it))
    {
        ++it;
    }

    auto it2 = it;
    if (it == source->end())
    {
        return Token(TokenType::Eof);
    }
    else if (*it == '_' || isalpha(*it)) // identifier or keyword
    {
        while(it2!=source->end() && (*it2 == '_' || isalnum(*it2))) ++it2;
        auto word = toString(it, it2);
        it = it2;
        auto item = dictionary.insert(word).first;
        auto type = Token::isKeyword(*item);
        if (type.first)
            return Token(type.second, &(*item));
        else
            return Token(TokenType::Identifier, &(*item));
    }
    else if (isdigit(*it)) // number, just support positive integer
    {
        while(it2!=source->end() && isdigit(*it2)) ++it2;
        auto word = toString(it, it2);
        it = it2;
        auto item = dictionary.insert(word).first;
        return Token(TokenType::Number, &(*item));
    }
    else if (isPunctuationCharacter(*it)) // punctuation character
    {
        while(it2!=source->end() && (isPunctuationCharacter(*it2))) ++it2;
        auto word = toString(it, it2);
        auto type = Token::isPunctuator(word);
        while (!type.first && it2!=it+1)
        {
            --it2;
            word = toString(it, it2);
            type = Token::isPunctuator(word);
        }
        if (!type.first)
        {
            std::cerr << "Invalid punctuator: " << word << std::endl;
            exit(-1);
            //throw std::exception();
        }
        it = it2;
        auto item = dictionary.insert(word).first;
        return Token(type.second, &(*item));
    }
    else
    {
        auto word = toString(it, it+1);
        std::cout << "Invalid character: " << word << std::endl;
        exit(-1);
    }
}

