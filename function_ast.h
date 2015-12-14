#ifndef FUNCTION_AST_H
#define FUNCTION_AST_H

#include "expr_ast.h"
#include "prototype_ast.h"
#include "symbol_table.h"

#include <memory>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

class FunctionAST {
public:
  FunctionAST(IRBuilder<> *builder,
              std::shared_ptr<Module> module,
              std::unique_ptr<PrototypeAST> proto,
              std::unique_ptr<ExprAST> body,
              SymbolTable &st);

    Value* codegen();
private:
  IRBuilder<> *_builder;
  std::shared_ptr<Module> _module;
  std::unique_ptr<PrototypeAST> _proto;
  std::unique_ptr<ExprAST> _body;
    SymbolTable &_st;
};
#endif // FUNCTION_AST_H

