#ifndef ENGINE_H
#define ENGINE_H

#include "lval.h"

struct mpc_ast_t;

class Engine {
public:
    Engine();

    //LValRef eval(mpc_ast_t *t);
    LValRef eval(LValRef);
protected:
    LValRef evalOp(LValRef parent, const char *op);
    LValRef evalSExpr(LValRef val);
};

#endif // ENGINE_H
