#include "binary_expr_ast.h"

BinaryExprAST::BinaryExprAST(char op,
    std::unique_ptr<ExprAST> LHS,
    std::unique_ptr<ExprAST> RHS) :
  _op(op),
  _LHS(std::move(LHS)),
  _RHS(std::move(RHS))
{}
