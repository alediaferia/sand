#include "engine.h"
#include "mpc.h"

Engine::Engine() {}

LValRef Engine::eval(mpc_ast_t *t) {
  
  /* If tagged as number return it directly. */
  if (strstr(t->tag, "number")) {
    /* Check if there is some error in conversion */
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? LVal::fromNum(x) : LVal::fromError(LVal::LERR_BAD_NUM);
  }

  /* The operator is always second child. */
  char* op = t->children[1]->contents;

  /* We store the third child in `x` */
  LValRef x = eval(t->children[2]);

  /* Iterate the remaining children and combining. */
  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = evalOp(x, op, eval(t->children[i]));
    i++;
  }

  return x;

}

LValRef Engine::evalOp(LValRef first, char *op, LValRef second) {
    if (first->_type == LVal::ERR) { return first; }
    if (second->_type == LVal::ERR) { return second; }

    /* Otherwise do maths on the number values */
    if (strcmp(op, "+") == 0) { return LVal::fromNum(first->_num + second->_num); }
    if (strcmp(op, "-") == 0) { return LVal::fromNum(first->_num - second->_num); }
    if (strcmp(op, "*") == 0) { return LVal::fromNum(first->_num * second->_num); }

    if (strcmp(op, "/") == 0) {
      /* If second operand is zero return error */
      return second->_num == 0
        ? LVal::fromError(LVal::LERR_DIV_ZERO)
        : LVal::fromNum(first->_num / second->_num);
    }

    return LVal::fromError(LVal::LERR_BAD_OP);
}

