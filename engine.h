#ifndef ENGINE_H
#define ENGINE_H

#include "lval.h"

struct mpc_ast_t;

class Engine {
public:
    Engine();

    LValRef eval(mpc_ast_t *t);
protected:
    LValRef evalOp(LValRef first, char *op, LValRef second);
};

#endif // ENGINE_H
