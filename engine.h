#ifndef ENGINE_H
#define ENGINE_H

#include "lval.h"

#include "function_ast.h"

#include <llvm/IR/Value.h>

using namespace llvm;

struct mpc_ast_t;

class Engine {
public:
    Engine();

    //LValRef eval(mpc_ast_t *t);
    LValRef eval(LValRef);

    Value* eval(std::unique_ptr<FunctionAST> fast);
    // void evaluate(Value *v);
protected:
    LValRef evalOp(LValRef parent, const char *op);
    LValRef evalSExpr(LValRef val);
};

#endif // ENGINE_H
