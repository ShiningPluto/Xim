//
// Created by ximu on 5/4/17.
//

#include <llvm/IR/Function.h>
#include "AST.h"

TypeRefAST::TypeRefAST(Token token)
    : token(token)
{

}

llvm::Type *TypeRefAST::genCode(llvm::LLVMContext& context)
{
    if (type == nullptr)
    {
        if (token.getType() == TokenType::Int)
            type = llvm::Type::getInt32Ty(context);
    }

    return type;
}

ReturnAST::ReturnAST(ExpressionAST * expr)
    : expr(expr)
{

}

IntegerAST::IntegerAST(llvm::ConstantInt * ptr)
{
    value = ptr;
}

llvm::Value *FunctionDefAST::genCode(llvm::IRBuilder<>& builder, llvm::Module* module)
{
    auto& context = module->getContext();
    auto llvm_func_type = proto->genCode(context);
    llvm_func = llvm::Function::Create(llvm_func_type, llvm::Function::LinkageTypes::ExternalLinkage, name.getValue(), module);

    // create function body
    {
        llvm::BasicBlock* entry = llvm::BasicBlock::Create(context, "entry", llvm_func);
        builder.SetInsertPoint(entry);

        // TODO: deal with the arguments allocation

        for (AST* stmt : body)
        {

        }
    }

    return nullptr;
}

llvm::FunctionType *FunctionProtoAST::genCode(llvm::LLVMContext& context)
{
    type = llvm::FunctionType::get(return_type->genCode(context), false);
    return type;
}
