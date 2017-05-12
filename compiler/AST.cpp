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
    : token(token), type(nullptr)
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

llvm::Type *TypeRefAST::getLLVMType()
{
    return type;
}

ReturnAST::ReturnAST(TypeRefAST* ret, ExpressionAST * expr)
    : expr(expr), ret_type_ast(ret)
{

}

llvm::Value *ReturnAST::genCode(llvm::IRBuilder<> &builder, llvm::Module *module)
{
    auto expr_value = expr->genCode(builder, module);
    auto ret_type = ret_type_ast->getLLVMType();
    llvm::Value* ret_value;
    if (expr_value->getType()->isIntegerTy())
    {
        if (expr_value->getType()->getIntegerBitWidth() > ret_type->getIntegerBitWidth())
        {
            // TODO: add warning for truncating value!
            ret_value =  builder.CreateTruncOrBitCast(expr_value, ret_type);
        }
        else
        {
            ret_value = builder.CreateZExtOrBitCast(expr_value, ret_type);
        }
    }
    else if (llvm::isa<llvm::GlobalVariable>(expr_value))
    {
        ret_value = builder.CreateLoad(expr_value);
        //ret_value = dynamic_cast<llvm::GlobalVariable*>(expr_value)->stripPointerCasts();
    }

    builder.CreateRet(ret_value);
    return ret_value;
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
            stmt->genCode(builder, module);
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

llvm::Value *VariableDefAST::genCode(llvm::IRBuilder<> &builder, llvm::Module *module)
{
    type->genCode(builder, module);
    auto value = initial_value->genCode(builder, module);

    // TODO: add support for local variable!
    module->getOrInsertGlobal(name.getValue(), type->getLLVMType());
    llvm::GlobalVariable *gv = module->getNamedGlobal(name.getValue());
    if (llvm::isa<llvm::ConstantInt>(value))
    {
        gv->setInitializer(reinterpret_cast<llvm::ConstantInt*>(value));
    }
    gv->setLinkage(llvm::GlobalVariable::InternalLinkage); // TODO: deal with ExternalLinkage

    return nullptr;
}

VariableRefAST::VariableRefAST(Token token)
    : token(token)
{

}

llvm::Value *VariableRefAST::genCode(llvm::IRBuilder<> &builder, llvm::Module *module)
{
    return module->getNamedGlobal(token.getValue());
}
