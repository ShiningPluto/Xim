#include "Lexer.h"

#include <locale>
#include <fstream>
#include <iostream>
#include <unordered_map>

namespace
{
    /// the ispunct in standard library can't handle char32_t correctly
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

    const std::unordered_map<int, std::string> token_spells
     {
        #define PUNCTUATOR(X, Y) {TokenType::X, Y},
        #define KEYWORD(X, Y) {TokenType::X, Y},
        #include "Token.def"
     };
}

Lexer::Lexer()
{
    line = 1;
    column = 1;
}

void Lexer::setSource(std::vector<char32_t> const& source)
{
    this->source = &source;
    it = source.begin();
}

Token Lexer::nextToken()
{
    if (!buffer.empty())
    {
        auto& t = buffer.front();
        buffer.pop();
        return t;
    }

    return consumeSource();
}

Token Lexer::peek()
{
    Token t = consumeSource();
    buffer.push(t);
    return t;
}

Token Lexer::consumeSource()
{
    // TODO: make sure that the input string dose not contain tab and the line end is \n
    while(it != source->end() && isspace(*it))
    {
        if (*it == ' ')
        {
            column += 1;
        }
        else if (*it == '\t')
        {
            column += 4;
        }
        else if (*it == '\n')
        {
            line += 1;
            column = 1;
        }
        ++it;
    }

    Token token(line, column);

    auto it2 = it;
    if (it == source->end())
    {
        return token;
    }
    else if (*it == '_' || isalpha(*it)) // identifier or keyword
    {
        while(it2!=source->end() && (*it2 == '_' || isalnum(*it2))) ++it2;
        auto word = toString(it, it2);
        column += it2 - it;
        it = it2;
        auto item = dictionary.insert(word).first;
        auto type = Token::isKeyword(*item);
        if (type.first)
            token.set(type.second, &(*item));
        else
            token.set(TokenType::Identifier, &(*item));
    }
    else if (isdigit(*it)) // number, just support positive integer
    {
        while(it2!=source->end() && isdigit(*it2)) ++it2;
        auto word = toString(it, it2);
        column += it2 - it;
        it = it2;
        auto item = dictionary.insert(word).first;
        token.set(TokenType::Number, &(*item));
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
        column += it2 - it;
        it = it2;
        auto item = dictionary.insert(word).first;
        token.set(type.second, &(*item));
    }
    else
    {
        auto word = toString(it, it+1);
        std::cout << "Invalid character: " << word << std::endl;
        exit(-1);
    }
    return token;
}

void Lexer::eatToken(TokenType type)
{
    Token t = nextToken();

    if (t.getType() != type)
    {
        std::cerr << t.getLine() << ":" << t.getColume() << " expect \"" << token_spells.at(type) << "\" \n";
        throw std::exception();
    }
}

