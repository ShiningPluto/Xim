//
// Created by ximu on 5/23/17.
//

#include "CodeGen.h"

llvm::Value *CodeGen::genCode(AST *st)
{
    switch (st->getType())
    {
        case ASTType::FunctionDef:
            genCode(reinterpret_cast<FunctionDefAST*>(st)); break;
        default:
            ;
    }
    return nullptr;
}

llvm::Value *CodeGen::genCode(FunctionDefAST *st)
{
    auto& context = module.getContext();
    auto llvm_func_type = getFunctionType(st->proto);
    auto llvm_func = llvm::Function::Create(llvm_func_type, llvm::Function::LinkageTypes::ExternalLinkage, st->name.getValue(), &module);

    llvm::BasicBlock* entry = llvm::BasicBlock::Create(context, "entry", llvm_func);
    builder.SetInsertPoint(entry);

    // deal with the arguments allocation
    if (!proto->parameter_types.empty())
    {
        auto it = llvm_func->arg_begin();
        for (int i=0; i<proto->parameter_types.size(); ++i)
        {
            llvm::AllocaInst* inst = builder.CreateAlloca(proto->parameter_types[i]->type, 0, proto->parameter_names[i]->c_str());
            builder.CreateStore(&*it, inst);
        }
    }

    // create function body
    {
        for (AST* stmt : body)
        {
            stmt->genCode(builder, module);
        }
    }

    return nullptr;
}

llvm::FunctionType *CodeGen::getFunctionType(FunctionProtoAST *st)
{
    st->return_type->genCode(builder, &module);
    if (st->parameter_types.empty())
    {
        type = llvm::FunctionType::get(return_type->type, false);
    }
    else
    {
        std::vector<llvm::Type*> args_llvm_type(parameter_types.size());
        for (size_t t = 0; t< parameter_types.size(); ++t)
        {
            parameter_types[t]->genCode(builder, module);
            args_llvm_type[t] = parameter_types[t]->getLLVMType();
        }

        type = llvm::FunctionType::get(return_type->getLLVMType(), args_llvm_type, false);
    }
    return nullptr;
}
