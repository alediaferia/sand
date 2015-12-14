#ifndef VAR_EXPR_AST_H
#define VAR_EXPR_AST_H

#include "expr_ast.h"
#include "symbol_table.h"

#include <string>

#include <llvm/IR/IRBuilder.h>

class VarExprAST : public ExprAST {
public:
    VarExprAST(IRBuilder<> *builder,
             const std::string &name,
               SymbolTable &st);
    
    Value* codegen() override;

private:
  IRBuilder<> *_builder;
  std::string _name;
  SymbolTable &_st;
};

#endif // VAR_EXPR_AST_H
