#include "parser.h"
#include "mpc.h"

#include "error.h"

#include <iostream>
#include <cstdlib>
#include <cstdio>

static const char *SAND_GRAMMAR =
"                                          \
  number : /-?[0-9]+/ ;                    \
  symbol : '+' | '-' | '*' | '/' ;         \
  sexpr  : '(' <expr>* ')' ;               \
  qexpr  : '{' <expr>* '}' ;               \
  expr   : <number> | <symbol> | <sexpr> ; \
  lispy  : /^/ <expr>* /$/ ;               \
";


static const char *SAND_STDIN_TAG = "<console>";

static const std::string def_id("def");
static const std::string extern_id("extern");

Parser::Parser() : _inputTag(nullptr) {
    _tags.number = mpc_new("number");
    _tags.symbol = mpc_new("symbol");
    _tags.sexpr  = mpc_new("sexpr");
    _tags.qexpr  = mpc_new("qexpr");
    _tags.expr   = mpc_new("expr");
    _tags.lispy  = mpc_new("lispy");

    mpca_lang(MPCA_LANG_DEFAULT,
    SAND_GRAMMAR,
    _tags.number, _tags.symbol, _tags.sexpr, _tags.qexpr, _tags.expr, _tags.lispy);
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
      6,
      _tags.number,
      _tags.symbol,
      _tags.sexpr,
      _tags.expr,
      _tags.qexpr,
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

int Parser::readToken(const std::string &input) {
  static int lastChr = ' '; // the last read char from input

  auto it = input.cbegin();
  auto end = input.cend();
  while (it != end && isspace(lastChr)) {
    lastChr = *(++it);
  }

  if (isalpha(lastChr)) { /// identifier: [a-zA-Z][a-zA-Z0-9]*
    _identifier = lastChr;  
    while (it != end && isalnum((lastChr = *(++it))))
      _identifier += lastChr;

    if (_identifier == def_id)
      return tok_def;
    if (_identifier == extern_id)
      return tok_extern;
    return tok_identifier;
  }

  if (isdigit(lastChr)) { // number [0-9]
    std::string num;
    do {
      num += lastChr;
    } while (it != end && isdigit((lastChr = *(++it))));
    _number = strtol(num.c_str(), nullptr, 10);
    
    return tok_number; 
  }

  if (lastChr == '#') {
    // just a loop for ignoring everything after the #
    while (it != end && (lastChr = *(++it)) != '\n')
      if (lastChr != '\r' && lastChr != EOF)
        continue;
  }

  if (it == end)
    return tok_eof;

  // don't know what to do with this char
  return lastChr;
}
