#ifndef ENGINE_H
#define ENGINE_H

#include "function_ast.h"

#include <llvm/IR/Value.h>

using namespace llvm;

struct mpc_ast_t;

class Engine {
public:
    Engine();

    Value* eval(std::unique_ptr<FunctionAST> fast);
    Value* eval(std::unique_ptr<ExprAST> expr);
    // void evaluate(Value *v);
protected:
};

#endif // ENGINE_H
