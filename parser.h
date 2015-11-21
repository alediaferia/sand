//
// Created by Alessandro Diaferia on 16/11/2015.
//

#ifndef PARSER_H
#define PARSER_H

#include "lval.h"

#include <memory>

struct mpc_parser_t;

class Parser {
public:
  enum Type {
    File,
    Stdin
};
  Parser(Type type);
  ~Parser();

  std::shared_ptr<Printable> parse(const std::string &input);

  const char* currentInputTag() const;
protected:
  Parser();

private:
  Type _type;

  struct {
    mpc_parser_t *number;
    mpc_parser_t *symbol;
    mpc_parser_t *sexpr;
    mpc_parser_t *expr;
    mpc_parser_t *lispy;
  } _tags;

  const char *_inputTag;
};

#endif //PARSER_H
