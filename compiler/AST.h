//
// Created by ximu on 5/4/17.
//

#pragma once

#include "Token.h"

#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/ADT/APSInt.h>

#include <string>
#include <vector>

enum ASTType
{
    TypeRef,
    Expression,
    AST_FUNCTION_DEF,
    AST_FUNCTION_PROTO,
    AST_RETURN,
    Integer
};

class AST
{
public:
    virtual int getType() const = 0;
};

class TypeRefAST : public AST
{
    Token token;
public:
    llvm::Type* type;

    TypeRefAST() = default;

    TypeRefAST(Token token);

    int getType() const override
    {
        return ASTType::TypeRef;
    }

    llvm::Type* genCode(llvm::LLVMContext& context);
};

class ExpressionAST : public AST
{
public:
    ExpressionAST() = default;

    int getType() const override
    {
        return ASTType::Expression;
    }
};

class FunctionProtoAST : public AST
{
public:
    std::vector<TypeRefAST*> parameter_types;
    std::vector<Token*> parameter_names;
    TypeRefAST* return_type;
    llvm::FunctionType* type;

    FunctionProtoAST() = default;

    int getType() const override
    {
        return ASTType::AST_FUNCTION_PROTO;
    }

    llvm::FunctionType* genCode(llvm::LLVMContext& context);
};

class FunctionDefAST : public AST
{
    std::vector<std::string> args_name;
public:
    Token name;
    std::vector<AST*> body;
    FunctionProtoAST* proto;
    llvm::Function* llvm_func;

    FunctionDefAST() = default;

    int getType() const override
    {
        return ASTType::AST_FUNCTION_DEF;
    }

    llvm::Value* genCode(llvm::IRBuilder<>& builder, llvm::Module* module);
};

class ReturnAST : public AST
{
    ExpressionAST* expr;
public:
    ReturnAST(ExpressionAST*);

    int getType() const override
    {
        return ASTType::AST_RETURN;
    }
};

class IntegerAST : public ExpressionAST
{
    llvm::Value* value;
    llvm::APSInt v;
public:
    IntegerAST(llvm::ConstantInt*);

    int getType() const override
    {
        return ASTType::Integer;
    }
};