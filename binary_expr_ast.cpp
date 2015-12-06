#include "binary_expr_ast.h"

#include "error.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

BinaryExprAST::BinaryExprAST(IRBuilder<> *builder,
    char op,
    std::unique_ptr<ExprAST> LHS,
    std::unique_ptr<ExprAST> RHS) :
  _builder(builder),
  _op(op),
  _LHS(std::move(LHS)),
  _RHS(std::move(RHS))
{}

Value* BinaryExprAST::codegen() {
  Value *L = _LHS->codegen();
  Value *R = _RHS->codegen();
  if (!L || !R)
    return nullptr;

  switch (_op) {
  case '+':
    return _builder->CreateAdd(L, R, "addtmp");
  case '-':
    return _builder->CreateSub(L, R, "subtmp");
  case '*':
    return _builder->CreateMul(L, R, "multmp");
  case '<':
    return _builder->CreateICmpULT(L, R, "cmptmp");
  default:
    return ErrorV("invalid binary operator");
  }
}
