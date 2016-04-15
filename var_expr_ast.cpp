#include "var_expr_ast.h"

#include "error.h"

VarExprAST::VarExprAST(IRBuilder<> *builder,
                       const std::string &name,
                       SymbolTable &st) :
_builder(builder),
  _name(name),
  _st(st)
{}

Value *VarExprAST::codegen() {
    Value *v = _st[_name];
    if (!v)
        return ErrorV("Unknown variable name");
    
    return v;
}
