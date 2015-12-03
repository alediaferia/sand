#ifndef VAR_EXPR_AST_H
#define VAR_EXPR_AST_H

#include "expr_ast.h"

#include <string>

class VarExprAST : public ExprAST {
public:
  VarExprAST(const std::string &name);

private:
  std::string _name;
};

#endif // VAR_EXPR_AST_H
