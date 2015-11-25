//
// Created by Alessandro Diaferia on 16/11/2015.
//

#include "lval.h"
#include "mpc.h"

#include <cstring>
#include <cstdlib>
#include <cstdio>

LVal::LVal() :
_type(Uninitialized),
_num(0) {

}

LVal::~LVal() {
    switch(_type) {
    case NUM:
    case ERR:
    case SYM:
        break;
    case SEXPR:
        break;
    }
}

LValRef LVal::fromNum(long num) {
    LVal *lv = new LVal();
    lv->_type = NUM;
    lv->_num = num;
    return LValRef(lv);
}

LValRef LVal::readNum(mpc_ast_t *val) {
    errno = 0;
    long x = std::strtol(val->contents, nullptr, 10);
    return errno != ERANGE ? LVal::fromNum(x) : LVal::fromError(LERR_BAD_NUM);
}

LValRef LVal::fromError(ErrorType type) {
  LVal *lv = new LVal();
  lv->_err = type;
  lv->_type = ERR;
  return LValRef(lv);
}

LValRef LVal::fromVal(mpc_ast_t *val) {
    if (strstr(val->tag, "number")) { return LVal::readNum(val); }
    if (strstr(val->tag, "symbol")) { return LVal::fromSymbol(std::string(val->contents)); }

    LVal *x = nullptr;
    if (strcmp(val->tag, ">") == 0) {
        x = new LVal();
        x->_type = LVal::SEXPR;
    }

    if (strcmp(val->tag, "sexpr")) {
        x = new LVal();
        x->_type = LVal::SEXPR;
    }

    for (int i = 0; i < val->children_num; i++) {
        if (strcmp(val->children[i]->contents, "(") == 0) { continue; }
        if (strcmp(val->children[i]->contents, ")") == 0) { continue; }
        if (strcmp(val->children[i]->contents, "}") == 0) { continue; }
        if (strcmp(val->children[i]->contents, "{") == 0) { continue; }
        if (strcmp(val->children[i]->tag,  "regex") == 0) { continue; }
        x->_lvals.push_back(LVal::fromVal(val->children[i]));
    }

    return LValRef(x);
}

LValRef LVal::fromSymbol(std::string &&symbol) {
  LVal *lv = new LVal();
  lv->_sym = std::move(symbol);
  lv->_type = SYM;
  return LValRef(lv);
}

std::string LVal::printable() const {
  char *buff = new char[MAX_PRINTABLE_BUFSIZE];
  switch (_type) {
  case NUM:
      std::snprintf(buff, MAX_PRINTABLE_BUFSIZE, "%li", _num);
      break;
  case ERR:
      std::snprintf(buff, MAX_PRINTABLE_BUFSIZE, "Error: %s", printableError().c_str());
      break;
  case SYM:
      std::snprintf(buff, MAX_PRINTABLE_BUFSIZE, "%s", _sym.c_str());
      break;
  case SEXPR:
      std::snprintf(buff, MAX_PRINTABLE_BUFSIZE, "(%s)", printableChildren().c_str());
      break;
  default:
      (void)0;
  }
  std::string str(buff);
  delete[] buff;
  return str;
}

std::string LVal::printableError() const {
  static std::string err_div_zero("Cannot divide by zero");
  static std::string err_bad_op("Unsupported operation");
  static std::string err_bad_num("Bad number");

  switch(_err) {
    case LERR_DIV_ZERO:
      return err_div_zero;
    case LERR_BAD_OP:
      return err_bad_op;
    case LERR_BAD_NUM:
      return err_bad_num;
    default:
      (void)0;
  }
  return "";
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

