#include "number_expr_ast.h"

#include <llvm/ADT/STLExtras.h>

NumberExprAST::NumberExprAST(long val) :
  _val(val)
{}

Value* NumberExprAST::codegen() {
  return ConstantFP::get(getGlobalContxt(), APLong(_val));
}
