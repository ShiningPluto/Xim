//
// Created by ximu on 5/4/17.
//

#include "AST.h"

TypeRefAST::TypeRefAST(Token token)
    : token(token)
{

}

ReturnAST::ReturnAST(ExpressionAST * expr)
    : expr(expr)
{

}

IntegerAST::IntegerAST(llvm::ConstantInt * ptr)
{
    value = ptr;
}
