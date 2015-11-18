#include "parser.h"

static const char *SAND_GRAMMAR =
"                                          \
  number : /-?[0-9]+/ ;                    \
  symbol : '+' | '-' | '*' | '/' ;         \
  sexpr  : '(' <expr>* ')' ;               \
  expr   : <number> | <symbol> | <sexpr> ; \
  lispy  : /^/ <expr>* /$/ ;               \
";


static const char *SAND_STDIN_TAG = "<console>";

Parser::Parser(Type type) :
    _type(type) {}
