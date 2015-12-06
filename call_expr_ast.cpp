#include "call_expr_ast.h"
#include "error.h"

#include <llvm/IR/Module.h>

#include <cstring>

CallExprAST::CallExprAST(IRBuilder<> *builder,
                         std::shared_ptr<Module> module,
                         const std::string &callee,
                         std::vector<std::unique_ptr<ExprAST>> args) :
  _builder(builder),
  _module(module),
  _callee(callee),
  _args(std::move(args))
{}

Value* CallExprAST::codegen() {
    // Look up the name in the global module table.
    Function *CalleeF = _module->getFunction(_callee);
    if (!CalleeF)
        return ErrorV("Unknown function referenced");
    
    // If argument mismatch error.
    if (CalleeF->arg_size() != _args.size())
        return ErrorV("Incorrect number of arguments passed");
    
    std::vector<Value *> ArgsV;
    for (unsigned i = 0, e = _args.size(); i != e; ++i) {
        ArgsV.push_back(_args[i]->codegen());
        if (!ArgsV.back())
            return nullptr;
    }
    
    return _builder->CreateCall(CalleeF, ArgsV, "calltmp");
}