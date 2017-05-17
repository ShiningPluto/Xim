//
// Created by ximu on 17-4-27.
//

#pragma once

#include "Lexer.h"
#include "Token.h"
#include "AST.h"

#include <string>
#include <memory>

class AST;

class Parser
{
    using ASTVector = std::vector<AST*>;
    using TokenVector = std::vector<Token>;

    Lexer lexer;
    std::string file_path;
    std::vector<char> source_u8;
    std::vector<char32_t> source_u32;

    std::vector<Token> tokens;
    std::vector<AST*> program;
    ASTVector ast_nodes;

public:
    Parser(std::string const& source);

    void run();

    void parse();

    void generate(std::string const& filename);

    AST* parseDefinition(TokenVector::iterator& it);

    FunctionDefAST* parseFunctionDef(TokenVector::iterator& it);

    ExpressionAST* parseExpression(TokenVector::iterator& it);

    VariableDefAST* parseVariableDef(TokenVector::iterator& it);

    VariableDefAST* parseStackVariableDef(TokenVector::iterator& it);
};