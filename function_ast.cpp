#include "function_ast.h"

#include <llvm/IR/Verifier.h>

FunctionAST::FunctionAST(IRBuilder<> *builder,
                         std::shared_ptr<Module> module,
                         std::unique_ptr<PrototypeAST> proto,
                         std::unique_ptr<ExprAST> body,
                         SymbolTable &st) :
  _builder(builder),
  _module(module),
  _proto(std::move(proto)),
  _body(std::move(body)),
  _st(st)
{}

Value* FunctionAST::codegen() {
    // First, check for an existing function from a previous 'extern' declaration.
    Function *TheFunction = _module->getFunction(_proto->name());
    
    if (!TheFunction)
        TheFunction = _proto->codegen();
    
    if (!TheFunction)
        return nullptr;
    
    // Create a new basic block to start insertion into.
    BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", TheFunction);
    _builder->SetInsertPoint(BB);
    
    // Record the function arguments in the NamedValues map.
    _st.clear();
    for (auto &Arg : TheFunction->args())
        _st[Arg.getName()] = &Arg;
    
    if (Value *RetVal = _body->codegen()) {
        // Finish off the function.
        _builder->CreateRet(RetVal);
        
        // Validate the generated code, checking for consistency.
        verifyFunction(*TheFunction); // IR/Verifier.h
        
        return TheFunction;
    }
    
    // Error reading body, remove function.
    TheFunction->eraseFromParent();
    return nullptr;
}
