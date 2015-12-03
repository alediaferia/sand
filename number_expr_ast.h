#ifndef NUMBER_EXPR_AST_H
#define NUMBER_EXPR_AST_H

#include "expr_ast.h"

class NumberExprAST : public ExprAST {
public: 
  NumberExprAST(long val);

private:
  long _val;
};

#endif // NUMBER_EXPR_AST
