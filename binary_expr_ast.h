#ifndef BINARY_EXPR_AST_H
#define BINARY_EXPR_AST_H

#include "expr_ast.h"

#include <memory>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

using namespace llvm;

class BinaryExprAST : public ExprAST {
public:
  BinaryExprAST(IRBuilder<> *,
      char op,
      std::unique_ptr<ExprAST> LHS,
      std::unique_ptr<ExprAST> RHS);

  Value* codegen() override;

private:
  IRBuilder<> *_builder;
  char _op;
  std::unique_ptr<ExprAST> _LHS;
  std::unique_ptr<ExprAST> _RHS;
};

#endif // BINARY_EXPR_AST_H
