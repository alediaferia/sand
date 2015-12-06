#ifndef CALL_EXPR_AST_H
#define CALL_EXPR_AST_H

#include "expr_ast.h"
#include <memory>
#include <vector>
#include <string>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

using namespace llvm;

class CallExprAST : public ExprAST {
public:
  CallExprAST(IRBuilder<> *builder,
              std::shared_ptr<Module> module,
              const std::string &callee,
              std::vector<std::unique_ptr<ExprAST>> args);
    
    Value* codegen() override;

private:
  IRBuilder<> *_builder;
  std::shared_ptr<Module> _module;
  std::string _callee;
  std::vector<std::unique_ptr<ExprAST>> _args;
};

#endif // CALL_EXPR_AST_H
