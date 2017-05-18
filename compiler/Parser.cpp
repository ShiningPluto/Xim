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
#include <unordered_map>

namespace
{
//    void assertTokenType(Token& token, TokenType type)
//    {
//        if (token.getType() != type)
//        {
//            std::cerr << "Expected token: \n";
//        }
//    }



//    void eatOperator(std::vector<Token>::iterator &it, TokenType type)
//    {
//        if (it->getType() == type)
//        {
//            ++it;
//        }
//        else
//        {
//            std::cerr << it->getLine() << ":" << it->getColume() << " expect \"" << token_spells.at(type) << "\" \n";
//            throw std::exception();
//        }
//    }

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
//    auto token = lexer.nextToken();
//    while (token.getType() != TokenType::Eof)
//    {
//        tokens.push_back(token);
//        std::cout << token;
//        token = lexer.nextToken();
//    }
//    std::cout << token << "\n";

    parse();
}

void Parser::generate(std::string const &filename)
{
    module = new llvm::Module("xim", context);
    for (AST* stmt : program)
    {
        stmt->genCode(builder, module);
    }

    std::error_code error_code;
    llvm::raw_fd_ostream out(filename, error_code, llvm::sys::fs::OpenFlags::F_RW);
    module->print(out, nullptr);
    module->dump();
}

void Parser::parse()
{
    auto t = lexer.nextToken();

    while (!t.is(TokenType::Eof))
    {
        if (t.is(TokenType::Def))
        {
            program.push_back(parseDefinition());

        }
        else if (t.is(TokenType::Var))
        {
            program.push_back(parseVariableDef());
        }
        else
        {
            throw std::logic_error("Parser::parse()");
        }

        t = lexer.nextToken();
    }
}

AST* Parser::parseDefinition()
{
    Token name = lexer.nextToken();
    lexer.eatToken(TokenType::Equal);
    FunctionDefAST* result = nullptr;

    Token t = lexer.nextToken();
    if (t.is(TokenType::Func))
    {
        result = parseFunctionDef();
        result->name = name;
    }
    else
    {
        throw std::logic_error("Unknown definition type");
    }

    return result;
}

FunctionDefAST* Parser::parseFunctionDef()
{
    auto result = new FunctionDefAST();
    result->proto = new FunctionProtoAST();
    lexer.eatToken(TokenType::LeftParen);
    auto t = lexer.nextToken();
    if (!t.is(TokenType::RightParen)) // parameters
    {
        while (true)
        {
            result->proto->parameter_types.push_back(new TypeRefAST(t));
            t = lexer.nextToken();
            if (t.is(TokenType::Identifier))
            {
                result->proto->parameter_names.push_back(&(t.getValue()));
            }
            else
            {
                result->proto->parameter_names.push_back(nullptr);
                if (t.is(TokenType::RightParen))
                {
                    break;
                }
                else if(t.is(TokenType::Comma))
                {
                    t = lexer.nextToken();
                }
                else
                {
                     throw std::logic_error("Incorrect function prototype");
                }
            }
        }
    }

    lexer.eatToken(TokenType::Arrow);

    result->proto->return_type = new TypeRefAST(lexer.nextToken());

    lexer.eatToken(TokenType::LeftBrace);
    t = lexer.nextToken();
    while (!t.is(TokenType::RightBrace))
    {
        if (t.is(TokenType::Return))
        {
            result->body.push_back(new ReturnAST(result->proto->return_type, parseExpression()));
        }
        else if (t.is(TokenType::Var))
        {
            result->body.push_back(parseStackVariableDef());
        }
        else
        {
            throw std::logic_error("Unknown statement for function body!");
        }
        t = lexer.nextToken();
    }

    return result;
}

ExpressionAST *Parser::parseExpression()
{
    ExpressionAST* result = nullptr;
    auto t = lexer.nextToken();

    if (t.is(TokenType::Number)) // integer literal
    {
        auto bitwidth = llvm::APInt::getBitsNeeded(llvm::StringRef(t.getValue()), 10) + 1;
        //bitwidth = (bitwidth + 31) / 32 * 32;
        llvm::APInt v(bitwidth, llvm::StringRef(t.getValue()), 10);
        result = new IntegerAST(llvm::ConstantInt::get(context, v));
    }
    else if(t.is(TokenType::Identifier)) // global variable
    {
        result = new VariableRefAST(t);
    }

    t = lexer.nextToken();
    if (t.is(TokenType::Plus) || t.is(TokenType::Star))
    {
        auto l = result;
        auto o = t;
        auto r = parseExpression();
        result = new BinaryOperationAST(o, l, r);
    }
    else if (!t.is(TokenType::SemiColon))
    {
        throw std::logic_error("Expression must end with semicolon!");
    }

    return result;
}

VariableDefAST* Parser::parseVariableDef()
{
    auto result = new VariableDefAST();
    auto t = lexer.nextToken();

    result->name = t;
    lexer.eatToken(TokenType::Colon);
    t = lexer.nextToken();
    result->type = new TypeRefAST(t);
    lexer.eatToken(TokenType::Equal);
    result->initial_value = parseExpression();
    return result;
}

VariableDefAST *Parser::parseStackVariableDef()
{
    return parseVariableDef();
}
