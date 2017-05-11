//
// Created by ximu on 5/4/17.
//

#include "AST.h"

#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>

//llvm::Value* convertType(llvm::Value* val, llvm::Type* to)
//{
//
//}


TypeRefAST::TypeRefAST(Token token)
    : token(token)
{

}

llvm::Value *TypeRefAST::genCode(llvm::IRBuilder<>& builder, llvm::Module* module)
{
    if (type == nullptr)
    {
        if (token.getType() == TokenType::Int)
            type = builder.getInt32Ty();
    }

    return nullptr;
}

ReturnAST::ReturnAST(ExpressionAST * expr)
    : expr(expr)
{

}

llvm::Value *ReturnAST::genCode(llvm::IRBuilder<> &builder, llvm::Module *module)
{
    //return builder.CreateRet(expr->genCode(builder, module));
    return expr->genCode(builder, module);
}

IntegerAST::IntegerAST(llvm::ConstantInt * ptr)
{
    value = ptr;
}

llvm::Value *IntegerAST::genCode(llvm::IRBuilder<> &builder, llvm::Module *module)
{
    return value;
}

llvm::Value *FunctionDefAST::genCode(llvm::IRBuilder<>& builder, llvm::Module* module)
{
    auto& context = module->getContext();
    proto->genCode(builder, module);
    auto llvm_func_type = proto->type;
    llvm_func = llvm::Function::Create(llvm_func_type, llvm::Function::LinkageTypes::ExternalLinkage, name.getValue(), module);

    // create function body
    {
        llvm::BasicBlock* entry = llvm::BasicBlock::Create(context, "entry", llvm_func);
        builder.SetInsertPoint(entry);

        // TODO: deal with the arguments allocation

        for (AST* stmt : body)
        {
            auto temp = stmt->genCode(builder, module);
            if (stmt->getType() == ASTType::AST_RETURN)
            {
                builder.CreateRet(builder.CreateTruncOrBitCast(temp, llvm_func_type->getReturnType()));
            }
        }
    }

    return nullptr;
}

llvm::Value *FunctionProtoAST::genCode(llvm::IRBuilder<> &builder, llvm::Module *module)
{
    return_type->genCode(builder, module);
    type = llvm::FunctionType::get(return_type->type, false);
    return nullptr;
}

llvm::Value *ExpressionAST::genCode(llvm::IRBuilder<> &builder, llvm::Module *module)
{
    return nullptr;
}
