#include "prototype_ast.h"

#include <memory>

PrototypeAST::PrototypeAST(const std::string &name, std::vector<std::string> args) :
  _name(name),
  _args(std::move(args))
{}
