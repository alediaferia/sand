//
// Created by Alessandro Diaferia on 16/11/2015.
//

#ifndef PARSER_H
#define PARSER_H

#include "lval.h"

struct mpc_parser_t;

class Parser {
public:
  enum Type {
    File,
    Stdin
  }
  Parser(Type type);
  ~Parser();

  void init();

  void parse(const char *input);
private:
  Type _type;

  struct {
    mpc_parser_t *number;
    mpc_parser_t *symbol;
    mpc_parser_t *sexpr;
    mpc_parser_t *expr;
    mpc_parser_t *lispy;
  } _tags;
};

#endif //PARSER_H
