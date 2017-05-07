//
// Created by ximu on 17-4-27.
//

#include "Parser.h"

#include "Token.h"
#include "AST.h"

#include <llvm/IR/Type.h>
#include <llvm/IR/LLVMContext.h>

#include <locale>
#include <fstream>
#include <iostream>
#include <map>

namespace
{
    void assertTokenType(Token& token, TokenType type)
    {
        if (token.getType() != type)
        {
            std::cerr << "Expected token: \n";
        }
    }

    void eatOperator(std::vector<Token>::iterator &it, TokenType type)
    {
        if (it->getType() == type)
        {
            ++it;
        }
        else
        {
            std::cerr << "Expected token: \n";
        }
    }

    llvm::LLVMContext context;


    std::map<TokenType, llvm::Type*> builtin_type_map;

    void constructBuiltinTypeMap(std::map<TokenType, llvm::Type*> & map)
    {
        map.insert(std::make_pair(TokenType::Int, llvm::Type::getInt32Ty(context)));
    }
}

Parser::Parser(std::string const& source)
    : file_path(source)
{
    std::ifstream input(file_path, std::ifstream::binary);
    if (!input.is_open())
    {
        throw std::exception();
    }
    input.seekg(0, input.end);
    int length = input.tellg();
    source_u8.resize(length);
    input.seekg(0, input.beg);
    input.read(source_u8.data(), length);
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

    lexer.setSource(source_u32);
}

void Parser::run()
{
    auto token = lexer.nextToken();
    while (token.getType() != TokenType::Eof)
    {
        tokens.push_back(token);
        std::cout << token;
        token = lexer.nextToken();
    }
    std::cout << token;

    parse();
}

void Parser::parse()
{
    auto it = tokens.begin();

    while (it !=)
    if (it->getType() == TokenType::Def) readDefinition(it+1);
}

AST* Parser::readDefinition(std::vector<Token>::iterator it)
{
    std::string const& name = it->getValue(); ++it;
    eatOperator(it, TokenType::Colon);

    if (it->getType()==TokenType::Func)
    {
        return readFunctionDef(it+1);
    }
}

AST* Parser::readFunctionDef(std::vector<Token>::iterator it)
{
    auto result = new FunctionDefAST();
    result->proto = new FunctionProtoAST();
    eatOperator(it, TokenType::LeftParen);
    if (it->getType()!=TokenType::RightParen) // parameters
    {
        while (true)
        {
            result->proto->parameter_types.push_back(new TypeRefAST(*it));
            ++it;
            if (it->getType()==TokenType::Identifier)
            {
                result->proto->parameter_names.push_back(it.base());
                ++it;
            }
            else
            {
                result->proto->parameter_names.push_back(nullptr)
            }

            if (it->getType() == TokenType::RightParen)
            {
                break;
            }
            else
            {
                eatOperator(it, TokenType::Comma);
            }
        }
    }
    eatOperator(it, TokenType::RightParen);

    eatOperator(it, TokenType::Arrow);

    result->proto->return_type = new TypeRefAST(*it);
    ++it;

    eatOperator(it, TokenType::LeftBrace);
    while (it->getType()!= TokenType::RightBrace)
    {
        result->body.push_back(parseExpression(it));
    }
}

ExpressionAST *Parser::parseExpression(std::vector<Token>::iterator &it)
{
    if (it->getType() == TokenType::Return)
    {
        ++it;
        return new ReturnAST(parseExpression(it));
    }
    return nullptr;
}
