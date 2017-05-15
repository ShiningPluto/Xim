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
    llvm::Value* ret_value(nullptr);
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
    }

    builder.CreateRet(ret_value);
    return ret_value;
}

IntegerAST::IntegerAST(llvm::ConstantInt * ptr)
{
    result = ptr;
}

llvm::Value *IntegerAST::genCode(llvm::IRBuilder<> &builder, llvm::Module *module)
{
    return result;
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

llvm::Value *VariableDefAST::genCode(llvm::IRBuilder<> &builder, llvm::Module *module)
{
    type->genCode(builder, module);
    auto value = initial_value->genCode(builder, module);

    // TODO: add support for local variable!
    module->getOrInsertGlobal(name.getValue(), type->getLLVMType());
    llvm::GlobalVariable *gv = module->getNamedGlobal(name.getValue());
    if (llvm::isa<llvm::ConstantInt>(value))
    {
//        if (value->getType()->getIntegerBitWidth() < type->getLLVMType()->getIntegerBitWidth())
//        {
//            value = builder.CreateZExtOrBitCast(value, type->getLLVMType());
//        }
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
    result = module->getNamedGlobal(token.getValue());
    if (llvm::isa<llvm::GlobalVariable>(result))
    {
        result = builder.CreateLoad(result);
    }
    return result;
}

llvm::Value *UnaryOperationAST::genCode(llvm::IRBuilder<> &builder, llvm::Module *module)
{
    operand->genCode(builder, module);
    return result;
}

llvm::Value *BinaryOperationAST::genCode(llvm::IRBuilder<> &builder, llvm::Module *module)
{
    auto lhv = lhs->genCode(builder, module);
    auto rhv = rhs->genCode(builder, module);

    auto lht = lhv->getType();
    auto rht = rhv->getType();
    if (lht->isIntegerTy() && lht->isIntegerTy())
    {
        if (lht->getIntegerBitWidth() < rht->getIntegerBitWidth())
        {
            lhv = builder.CreateZExtOrBitCast(lhv, rht);
        }
        else
        {
            rhv = builder.CreateZExtOrBitCast(rhv, lht);
        }
    }
    else
    {
        // TODO: deal with float!
    }

    if (op.getType() == TokenType::Plus)
    {
        result = builder.CreateAdd(lhv, rhv);
    }
    else if (op.getType() == TokenType::Minus)
    {
        result = builder.CreateSub(lhv, rhv);
    }
    else if (op.getType() == TokenType::Star)
    {
        result = builder.CreateMul(lhv, rhv);
    }
    else if (op.getType() == TokenType::Slash)
    {
        result = builder.CreateSDiv(lhv, rhv);
    }
    return result;
}

BinaryOperationAST::BinaryOperationAST(Token o, ExpressionAST *l, ExpressionAST *r)
    : op(o), lhs(l), rhs(r)
{

}
