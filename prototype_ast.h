#ifndef PROTOTYPE_AST_H
#define PROTOTYPE_AST_H

#include <vector>
#include <string>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Module.h>

using namespace llvm;

class PrototypeAST {
public:
  PrototypeAST(IRBuilder<> *builder,
               std::shared_ptr<Module> module,
               const std::string &name,
      std::vector<std::string> args);
    
    Function *codegen();
    
    std::string name() const;

private:
  IRBuilder<> *_builder;
  std::shared_ptr<Module> _module;
  std::string _name;
  std::vector<std::string> _args;
};

#endif // PROTOTYPE_AST_H
