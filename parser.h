//
// Created by Alessandro Diaferia on 16/11/2015.
//

#ifndef PARSER_H
#define PARSER_H

#include "lval.h"
#include "either.h"
#include "error.h"

#include <memory>

struct mpc_parser_t;
struct mpc_ast_t;

enum Token {
  tok_eof = -1,

  // commands
  tok_def = -2,
  tok_extern = -3,

  // primary
  tok_identifier = -4,
  tok_number = -5
};

class Parser {
public:
  enum Type {
    File,
    Stdin
};
  Parser(Type type);
  ~Parser();

  Either<LValRef, ErrorRef> parse(const std::string &input);

  const char* currentInputTag() const;
protected:
  Parser();

  Token readToken(const std::string &input);

private:
  Type _type;

  struct {
    mpc_parser_t *number;
    mpc_parser_t *symbol;
    mpc_parser_t *sexpr;
    mpc_parser_t *qexpr;
    mpc_parser_t *expr;
    mpc_parser_t *lispy;
  } _tags;

  const char *_inputTag;
  std::string _identifier;
  long _number;
};

#endif //PARSER_H
