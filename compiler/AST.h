//
// Created by ximu on 5/4/17.
//

#pragma once

#include <string>
#include <vector>

enum ASTType
{
    AST_FUNCTION_DEF,
    AST_RETURN
};

class AST
{
public:
    virtual int getType() const = 0;
};

class FunctionDefAST : public AST
{
    std::string name;
    std::vector<std::string> args_name;
    std::vector<AST*> body;
public:
    FunctionDefAST(std::string, std::vector<std::string>, std::vector<AST*>);

    int getType() const override
    {
        return ASTType::AST_FUNCTION_DEF;
    }
};

class ReturnAST : public AST
{
    AST* expr;
public:
    ReturnAST(AST*);

    int getType() const override
    {
        return ASTType::AST_RETURN;
    }
};