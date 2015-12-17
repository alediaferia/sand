#include "number_expr_ast.h"

#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>

NumberExprAST::NumberExprAST(long val) :
  _val(val)
{
}

Value* NumberExprAST::codegen() {
    return ConstantInt::get(getGlobalContext(), APInt(sizeof(_val) * 8, _val, true));
}
