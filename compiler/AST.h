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
    Integer,
    UnaryOperation,
    BinaryOperation,
    FunctionDef,
    FunctionProto,
    FunctionCall,
    ReturnStatement,
    VariableDef,
    VariableRef
};

class AST
{
public:
    virtual int getType() const = 0;
    virtual llvm::Value* genCode(llvm::IRBuilder<>& builder, llvm::Module* module) = 0;
};

class TypeRefAST : public AST
{
    Token token;
public:
    llvm::Type* type;

    TypeRefAST(Token token);

    int getType() const override
    {
        return ASTType::TypeRef;
    }

    llvm::Value* genCode(llvm::IRBuilder<>& builder, llvm::Module* module) override;

    llvm::Type* getLLVMType();
};

class FunctionProtoAST : public AST
{
public:
    std::vector<TypeRefAST*> parameter_types;
    std::vector<std::string const*> parameter_names;
    TypeRefAST* return_type;
    llvm::FunctionType* type;

    FunctionProtoAST() = default;

    int getType() const override
    {
        return ASTType::FunctionProto;
    }

    llvm::Value* genCode(llvm::IRBuilder<>& builder, llvm::Module* module) override;
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
        return ASTType::FunctionDef;
    }

    llvm::Value* genCode(llvm::IRBuilder<>& builder, llvm::Module* module) override;
};

class ExpressionAST : public AST
{
protected:
    llvm::Value* result;
public:
    ExpressionAST() = default;

    int getType() const override
    {
        return ASTType::Expression;
    }

};

class IntegerAST : public ExpressionAST
{
    llvm::APSInt v;
public:
    IntegerAST(llvm::ConstantInt*);

    int getType() const override
    {
        return ASTType::Integer;
    }

    llvm::Value* genCode(llvm::IRBuilder<>& builder, llvm::Module* module) override;
};

class UnaryOperationAST : public ExpressionAST
{
    Token op;
    ExpressionAST* operand;
public:

    int getType() const override
    {
        return ASTType::UnaryOperation;
    }

    llvm::Value* genCode(llvm::IRBuilder<>& builder, llvm::Module* module) override;
};

class BinaryOperationAST : public ExpressionAST
{
    Token op;
    ExpressionAST* lhs;
    ExpressionAST* rhs;
public:
    BinaryOperationAST(Token o, ExpressionAST* l, ExpressionAST* r);

    int getType() const override
    {
        return ASTType::BinaryOperation;
    }

    llvm::Value* genCode(llvm::IRBuilder<>& builder, llvm::Module* module) override;
};

class FunctionCallAST : public ExpressionAST
{
public:
    Token name;
    std::vector<ExpressionAST*> args;

    FunctionCallAST() = default;

    int getType() const override
    {
        return ASTType::FunctionCall;
    }

    llvm::Value* genCode(llvm::IRBuilder<>& builder, llvm::Module* module) override;
};

class ReturnAST : public AST
{
    ExpressionAST* expr;
public:
    TypeRefAST* ret_type_ast;
    ReturnAST(TypeRefAST*, ExpressionAST*);

    int getType() const override
    {
        return ASTType::ReturnStatement;
    }

    llvm::Value* genCode(llvm::IRBuilder<>& builder, llvm::Module* module) override;
};

class VariableDefAST : public AST
{
public:
    Token name;
    TypeRefAST* type;
    ExpressionAST* initial_value;

    VariableDefAST() = default;

    int getType() const override
    {
        return ASTType::VariableDef;
    }

    llvm::Value* genCode(llvm::IRBuilder<>& builder, llvm::Module* module) override;
};

class VariableRefAST : public ExpressionAST
{
    Token token;
public:
    VariableRefAST(Token token);

    int getType() const override
    {
        return ASTType::VariableRef;
    }

    llvm::Value* genCode(llvm::IRBuilder<>& builder, llvm::Module* module) override;
};