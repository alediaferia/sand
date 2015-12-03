#include "function_ast.h"

FunctionAST::FunctionAST(std::unique_ptr<PrototypeAST> proto, std::unique_ptr<ExprAST> body) :
  _proto(std::move(proto)),
  _body(std::move(body))
{}
