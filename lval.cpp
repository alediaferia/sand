//
// Created by Alessandro Diaferia on 16/11/2015.
//

#include "lval.h"
#include "mpc.h"

#include <cstring>
#include <cstdlib>

LVal::~LVal() {
    switch(_type) {
    case LVAL_NUM:
    case LVAL_ERR:
    case LVAL_SYM:
        break;
    case LVAL_SEXPR:
        auto it = _lvals.cbegin();
        auto end = _lvals.cend();
        for (; it != end; ++it) {
            delete *it;
        }
    }
}

LVal* LVal::fromNum(long num) {
    LVal *lv = new LVal();
    lv->_type = LVAL_NUM;
    lv->_num = num;
    return lv;
}

LVal *LVal::readNum(mpc_ast_t *val) {
    errno = 0;
    long x = std::strtol(val->contents, nullptr, 10);
    return errno != ERANGE ? LVal::fromNum(x) : LVal::fromError("invalid number");
}

LVal *LVal::fromError(std::string &&message) {
  LVal *lv = new LVal();
  lv->_err = std::move(message);
  lv->_type = LVAL_ERR;
  return lv;
}

LVal *LVal::fromVal(mpc_ast_t *val) {
    if (strstr(val->tag, "number")) { return LVal::readNum(val); }
    if (strstr(val->tag, "symbol")) { return LVal::read }
}

LVal *LVal::fromSymbol(std::string &&symbol) {
  LVal *lv = new LVal();
  lv->_sym = std::move(symbol);
  lv->_type = LVAL_SYM;
  return lv;
}

std::string LVal::printable() const {
  char *buff = new char[MAX_PRINTABLE_BUFSIZE];
  switch (_type) {
  case LVAL_NUM:
      snprintf(buff, MAX_PRINTABLE_BUFSIZE, "%li", _num);
      break;
  case LVAL_ERR:
      snprintf(buff, MAX_PRINTABLE_BUFSIZE, "Error: %s", _err.c_str());
      break;
  case LVAL_SYM:
      snprintf(buff, MAX_PRINTABLE_BUFSIZE, "%s", _sym.c_str());
      break;
  case LVAL_SEXPR:
      snprintf(buff, MAX_PRINTABLE_BUFSIZE, "%s", printableChildren().c_str());
      break;
  }
  std::string str(buff);
  delete[] buff;
  return str;
}

std::string LVal::printableChildren() const {
    auto it = _lvals.cbegin();
    auto end  = _lvals.cend();

    std::string str;
    for (; it != end; ++it) {
        str += (*it)->printable();
        str += " ";
    }

    return str;
}
