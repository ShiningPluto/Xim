//
// Created by ximu on 17-4-27.
//

#include "Parser.h"

#include "Token.h"
#include "AST.h"

#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/ADT/APInt.h>
#include <llvm/ADT/StringRef.h>

#include <locale>
#include <fstream>
#include <iostream>
#include <map>

namespace
{
//    void assertTokenType(Token& token, TokenType type)
//    {
//        if (token.getType() != type)
//        {
//            std::cerr << "Expected token: \n";
//        }
//    }

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
    llvm::IRBuilder<> builder(context);
    llvm::Module * module;

    std::map<TokenType, llvm::Type*> builtin_type_map;

    void constructBuiltinTypeMap(std::map<TokenType, llvm::Type*> & map)
    {
        map.insert(std::make_pair(TokenType::Int, builder.getInt32Ty()));
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

    constructBuiltinTypeMap(builtin_type_map);
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
    std::cout << token << "\n";

    parse();

    module = new llvm::Module("test", context);
    for (AST* stmt : program)
    {
        stmt->genCode(builder, module);
    }

    std::error_code error_code;
    llvm::raw_fd_ostream out("return.ll", error_code, llvm::sys::fs::OpenFlags::F_RW);
    module->print(out, nullptr);
    module->dump();
}

void Parser::parse()
{
    auto it = tokens.begin();

    while (it != tokens.end())
    {
        if (it->getType() == TokenType::Def)
        {
            ++it;
            program.push_back(parseDefinition(it));
        }
    }
}

AST* Parser::parseDefinition(std::vector<Token>::iterator& it)
{
    auto name = *it; ++it;
    eatOperator(it, TokenType::Colon);
    FunctionDefAST* result = nullptr;

    if (it->getType()==TokenType::Func)
    {
        ++it;
        result = parseFunctionDef(it);
        result->name = name;
    }

    return result;
}

FunctionDefAST* Parser::parseFunctionDef(std::vector<Token>::iterator& it)
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
                result->proto->parameter_names.push_back(nullptr);
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
        if (it->getType() == TokenType::Return)
        {
            ++it;
            result->body.push_back(new ReturnAST(parseExpression(it)));
        }
        else
        {
            result->body.push_back(parseExpression(it));
        }
    }
    eatOperator(it, TokenType::RightBrace);

    return result;
}

ExpressionAST *Parser::parseExpression(std::vector<Token>::iterator &it)
{
    if (it->getType() == TokenType::Number)
    {
        llvm::APInt v(64, llvm::StringRef(it->getValue()), 10);
        ++it;
        eatOperator(it, TokenType::SemiColon);
        return new IntegerAST(llvm::ConstantInt::get(context, v));
    }
    return nullptr;
}
