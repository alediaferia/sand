#ifndef EXPR_AST_H
#define EXPR_AST_H

#include <llvm/IR/Value.h>
using namespace llvm;
class ExprAST {
public:
  virtual ~ExprAST() {}
    virtual Value* codegen() = 0;
};

#endif // EXPR_AST_H
