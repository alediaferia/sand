#ifndef CALL_EXPR_AST_H
#define CALL_EXPR_AST_H

#include "expr_ast.h"
#include <memory>
#include <vector>
#include <string>

class CallExprAST : public ExprAST {
public:
  CallExprAST(const std::string &callee,
      std::vector<std::unique_ptr<ExprAST>> args);

private:
  std::string _callee;
  std::vector<std::unique_ptr<ExprAST>> _args;
};

#endif // CALL_EXPR_AST_H
