//
// Created by Alessandro Diaferia on 16/11/2015.
//

#ifndef LVAL_H
#define LVAL_H

#include <string>
#include <list>

#include "printable.h"

struct mpc_ast_t;

class LVal : public Printable {
public:
  enum Type { LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR };

  LVal();
  LVal(const LVal &);
  ~LVal();

  static LVal *fromVal(mpc_ast_t *val);
  static LVal *fromNum(long num);
  static LVal *fromError(std::string &&message);
  static LVal *fromSymbol(std::string &&symbol);
  static LVal *readNum(mpc_ast_t *val);

  std::string printable() const;

private:
  std::string printableChildren() const;

private:
  Type _type;
  long _num;
  std::string _err;
  std::string _sym;
  int _count;
  std::list<LVal *> _lvals;

  friend class Parser;
};

#endif // LVAL_H
