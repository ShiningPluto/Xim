//
// Created by ximu on 4/20/17.
//

#include "Lexer.h"

#include "Token.h"

#include <sstream>
#include <locale>
#include <cctype>
#include <fstream>

Lexer::Lexer(std::shared_ptr<std::string> source)
    : file_path(source)
{
    std::ifstream input(*file_path, std::ifstream::binary);
    input.open(*file_path);
    input.seekg(0, input.end);
    source_u8.resize(input.tellg());
    input.seekg(0, input.beg);
    input.read(source_u8.data(), source_u8.size());
    input.close();

    std::locale locale;
    using facet_type = std::codecvt<char32_t, char, std::mbstate_t>;
    const facet_type& facet = std::use_facet<facet_type>(locale);
    source_u32.resize(source_u8.size());
    std::mbstate_t state{};
    const char* from_next;
    char32_t* to_next;
    auto result = facet.in(state, source_u8.data(), source_u8.data() + source_u8.size(), from_next,
    source_u32.data(), source_u32.data()+source_u32.size(), to_next);
    if (result!=facet_type::ok)
    {
        // TODO: error handle!
    }
    source_u32.resize(to_next - source_u32.data());
    it = source_u32.begin();
}

Lexer::~Lexer()
{

}

Token Lexer::nextToken()
{

    if (it == source_u32.end())
    {
        return Token(TokenType::Eof);
    }
    else
    {
        char32_t tt;
        char32_t c = *it;
        if (c == '_' || isalpha(c))
        {
            std::ostringstream s;
            while(c=='_' || isalpha(c) || isdigit(c))
            {
                s.put(static_cast<char>(input.get()));
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

