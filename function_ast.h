#ifndef FUNCTION_AST_H
#define FUNCTION_AST_H

#include "expr_ast.h"
#include "prototype_ast.h"
#include <memory>

class FunctionAST {
public:
  FunctionAST(std::unique_ptr<PrototypeAST> proto, std::unique_ptr<ExprAST> body);

private:
  std::unique_ptr<PrototypeAST> _proto;
  std::unique_ptr<ExprAST> _body;
};
#endif // FUNCTION_AST_H

