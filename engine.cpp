#include "engine.h"
#include "mpc.h"

Engine::Engine() {}

//LValRef Engine::eval(mpc_ast_t *t) {
//  
//  /* If tagged as number return it directly. */
//  if (strstr(t->tag, "number")) {
//    /* Check if there is some error in conversion */
//    errno = 0;
//    long x = strtol(t->contents, NULL, 10);
//    return errno != ERANGE ? LVal::fromNum(x) : LVal::fromError(LVal::LERR_BAD_NUM);
//  }
//
//  /* The operator is always second child. */
//  char* op = t->children[1]->contents;
//
//  /* We store the third child in `x` */
//  LValRef x = eval(t->children[2]);
//
//  /* Iterate the remaining children and combining. */
//  int i = 3;
//  while (strstr(t->children[i]->tag, "expr")) {
//    x = evalOp(x, op, eval(t->children[i]));
//    i++;
//  }
//
//  return x;
//}

LValRef Engine::eval(LValRef val) {
  if (val->_type == LVal::SEXPR) { return evalSExpr(val); }

  return val;
}

LValRef Engine::evalSExpr(LValRef val) {
  auto it = val->lvals().begin();
  for (; it != val->lvals().end(); ++it) {
    (*it) = eval(*it);  
  }

  it = val->lvals().begin();
  for (; it != val->lvals().end(); ++it) {
    if ((*it)->_type == LVal::ERR) {
        return *(val->lvals().erase(it));
    }
  }

  if (val->lvals().size() == 0) { return val; }
  if (val->lvals().size() == 1) { return val->pop(); }

  LValRef f = val->pop();
  if (f->_type != LVal::SYM) {
    return LVal::fromError(LVal::LERR_BAD_SEXPR_FORMAT);
  }
  LValRef result = evalOp(val, f->_sym.c_str());
  return result;
}

LValRef Engine::evalOp(LValRef parent, const char *op) {
    auto it = parent->lvals().cbegin();
    auto end = parent->lvals().cend();
    for (; it != end; ++it) {
      if ((*it)->_type != LVal::NUM) {
        return LVal::fromError(LVal::LERR_BAD_NUM);
      }
    }
    LValRef first;
    if ((strcmp(op, "-") == 0) && parent->lvals().size() == 0) {
      first = parent;
      first->_num = -(first->_num);
    } else {
      first = parent->pop();
    }

    while (parent->_lvals.size()) {
      LValRef second = parent->pop();

      if (strcmp(op, "+") == 0) { first->_num += second->_num; }
      if (strcmp(op, "-") == 0) { first->_num -= second->_num; }
      if (strcmp(op, "*") == 0) { first->_num *= second->_num; }

      if (strcmp(op, "/") == 0) {
        /* If second operand is zero return error */
        if (second->_num == 0) {
          first = LVal::fromError(LVal::LERR_DIV_ZERO);
          break;
        }
        first->_num /= second->_num;
      }
    }

    return first;
}

Value* Engine::eval(std::unique_ptr<FunctionAST> fast) {
    return fast->codegen();
}

Value* Engine::eval(std::unique_ptr<ExprAST> expr) {
    return expr->codegen();
}

