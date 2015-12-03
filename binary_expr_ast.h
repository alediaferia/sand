#ifndef BINARY_EXPR_AST_H
#define BINARY_EXPR_AST_H

#include "expr_ast.h"

#include <memory>

class BinaryExprAST : public ExprAST {
public:
  BinaryExprAST(char op,
      std::unique_ptr<ExprAST> LHS,
      std::unique_ptr<ExprAST> RHS);

private:
  char _op;
  std::unique_ptr<ExprAST> _LHS;
  std::unique_ptr<ExprAST> _RHS;
};

#endif // BINARY_EXPR_AST_H
