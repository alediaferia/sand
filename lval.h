//
// Created by Alessandro Diaferia on 16/11/2015.
//

#ifndef LVAL_H
#define LVAL_H

#include <string>
#include <list>

#include "printable.h"

struct mpc_ast_t;
class LVal;
typedef std::shared_ptr<LVal> LValRef;

class LVal : public Printable {
public:
  enum Type {
    Uninitialized = 0,
    NUM, ERR, SYM, SEXPR 
  };
  enum ErrorType {
    NOERR = 0,
    LERR_DIV_ZERO,
    LERR_BAD_OP,
    LERR_BAD_NUM,
    LERR_BAD_SEXPR_FORMAT
  };

  LVal();
  //LVal(const LVal &);
  ~LVal();

  static LValRef fromVal(mpc_ast_t *val);
  static LValRef fromNum(long num);
  static LValRef fromError(ErrorType type);
  static LValRef fromSymbol(std::string &&symbol);
  static LValRef readNum(mpc_ast_t *val);

  std::string printable() const;

  LValRef pop();

protected:
  std::list<LValRef>& lvals();

private:
  std::string printableChildren() const;
  std::string printableError() const;

private:
  Type _type;
  long _num;
  ErrorType _err;
  std::string _sym;
  std::list<LValRef> _lvals;

  friend class Engine;
};

#endif // LVAL_H
