//
// Created by ximu on 5/23/17.
//

#pragma once

#include "AST.h"

#include <llvm/IR/Module.h>

class CodeGen
{
    llvm::IRBuilder<> builder;
    llvm::Module& module;
public:
    CodeGen(llvm::Module& m)
        : module(m), builder(m.getContext())
    {
    }

    llvm::Value* genCode(AST* st);

    llvm::Value* genCode(FunctionDefAST* st);

private:
    llvm::FunctionType* getFunctionType(FunctionProtoAST* st);
};
