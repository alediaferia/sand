#include "parser.h"
#include "mpc.h"

#include "error.h"

#include <iostream>
#include <cstdlib>

static const char *SAND_GRAMMAR =
"                                          \
  number : /-?[0-9]+/ ;                    \
  symbol : '+' | '-' | '*' | '/' ;         \
  sexpr  : '(' <expr>* ')' ;               \
  expr   : <number> | <symbol> | <sexpr> ; \
  lispy  : /^/ <expr>* /$/ ;               \
";


static const char *SAND_STDIN_TAG = "<console>";

Parser::Parser() : _inputTag(nullptr) {
    _tags.number = mpc_new("number");
    _tags.symbol = mpc_new("symbol");
    _tags.sexpr  = mpc_new("sexpr");
    _tags.expr   = mpc_new("expr");
    _tags.lispy  = mpc_new("lispy");

    mpca_lang(MPCA_LANG_DEFAULT,
    SAND_GRAMMAR,
    _tags.number, _tags.symbol, _tags.sexpr, _tags.expr, _tags.lispy);
}

Parser::Parser(Type type) : Parser() {
  _type = type;
  switch (type) {
  case Stdin:
    _inputTag = SAND_STDIN_TAG;
    break;
  default:
    std::cerr << "Unsupported";
    abort();
  }
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

Either<LValRef,ErrorRef> Parser::parse(const std::string &input) {
  mpc_result_t r;
  if (mpc_parse(currentInputTag(), input.c_str(), _tags.lispy, &r)) {
    LValRef lval = LVal::fromVal((mpc_ast_t*)r.output);
    return Either<LValRef,ErrorRef>::Left(lval);
  }

  // A parsing error occurred
  char *err_str = mpc_err_string(r.error);
  std::string error(err_str);
  free(err_str);
  mpc_err_delete(r.error);

  return Either<LValRef,ErrorRef>::Right(ErrorRef(new Error(error)));
}

const char* Parser::currentInputTag() const {
    return SAND_STDIN_TAG;
}
