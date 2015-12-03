#include "call_expr_ast.h"

CallExprAST::CallExprAST(const std::string &callee, std::vector<std::unique_ptr<ExprAST>> args) :
  _callee(callee),
  _args(std::move(args))
{}

