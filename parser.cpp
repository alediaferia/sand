#include "parser.h"
#include "mpc.h"

#include <iostream>

static const char *SAND_GRAMMAR =
"                                          \
  number : /-?[0-9]+/ ;                    \
  symbol : '+' | '-' | '*' | '/' ;         \
  sexpr  : '(' <expr>* ')' ;               \
  expr   : <number> | <symbol> | <sexpr> ; \
  lispy  : /^/ <expr>* /$/ ;               \
";


static const char *SAND_STDIN_TAG = "<console>";

Parser::Parser(Type type) : _type(type) {

}

Parser::~Parser() {
    mpc_cleanup(
      5,
      _tags.number,
      _tags.symbol,
      _tags.sexpr,
      _tags.expr,
      _tags.lispy
    );
}

void Parser::init() {

}

void Parser::parse(const char *input) {
    mpc_result_t r;
}
