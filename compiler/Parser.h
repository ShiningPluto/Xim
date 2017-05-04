//
// Created by ximu on 17-4-27.
//

#pragma once

#include "Lexer.h"
#include "Token.h"

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

public:
    Parser(std::string const& source);

    void run();

    void parse();

    void readDefinition(TokenVector::iterator it);

    void readFunctionDef(TokenVector::iterator it);
};