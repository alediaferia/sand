#include "parser.h"
#include "mpc.h"

#include "number_expr_ast.h"
#include "var_expr_ast.h"
#include "call_expr_ast.h"
#include "error.h"

#include <iostream>
#include <cstdlib>
#include <cstdio>

#include <map>

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

static const std::map<char, int> BIN_OP_PRECEDENCE = {
    {'<', 10},
    {'+', 20},
    {'-', 30},
    {'*', 40},
};

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

//Either<LValRef,ErrorRef> Parser::parse(const std::string &input) {
//  mpc_result_t r;
//  if (mpc_parse(currentInputTag(), input.c_str(), _tags.lispy, &r)) {
//    LValRef lval = LVal::fromVal((mpc_ast_t*)r.output);
//    return Either<LValRef,ErrorRef>::Left(lval);
//  }
//
//  // A parsing error occurred
//  char *err_str = mpc_err_string(r.error);
//  std::string error(err_str);
//  free(err_str);
//  mpc_err_delete(r.error);
//
//  return Either<LValRef,ErrorRef>::Right(ErrorRef(new Error(error)));
//}

const char* Parser::currentInputTag() const {
    return SAND_STDIN_TAG;
}

std::unique_ptr<FunctionAST> Parser::parseDefinition(s_cursor_t &it, const s_cursor_t &end) {
    readToken(it, end); // eat 'def'

    auto proto = parsePrototype(it, end);
    if (!proto)
        return nullptr;

    if (auto e = parseExpression(it, end))
        return llvm::make_unique<FunctionAST>(std::move(proto), std::move(e));

    return nullptr; // no definition so far
}

std::unique_ptr<ExprAST> Parser::parseExpression(s_cursor_t &it, const s_cursor_t &end) {
    auto lhs = parsePrimary(it, end);
    if (!lhs)
        return nullptr;

    return parseBinOpRHS(it, end);
}

std::unique_ptr<ExprAST> Parser::parseBinOpRHS(s_cursor_t &it, const s_cursor_t &end) {

}

std::unique_ptr<ExprAST> Parser::parsePrimary(s_cursor_t &it, const s_cursor_t &end) {
    switch (_lastTok) {
        case tok_identifier:
            return parseIdentifierExpr(it, end);
        case tok_number:
            return parseNumberExpr(it, end);
        case '(':
            return parseParenExpr(it, end);
        default:
            return Error("unknown token when expecting an expression");
    }
}

std::unique_ptr<ExprAST> Parser::parseIdentifierExpr(s_cursor_t &it, const s_cursor_t &end) {
    std::string idName = _identifier;
    readToken(it, end);

    if (_lastTok != '(')
        return llvm::make_unique<VarExprAST>(idName);

    readToken(it, end);

    std::vector<std::unique_ptr<ExprAST>> args;
    if (_lastTok != ')') {
        while (true) {
            if (auto arg = parseExpression(it, end))
                args.push_back(std::move(arg));
            else
                return nullptr;

            if (_lastTok == ')')
                break;

            if (_lastTok != ',')
                return Error("Expected ')' or ',' in argument list");

            readToken(it, end);
        }
    }

    readToken(it, end); // eat ')'

    return llvm::make_unique<CallExprAST>(idName, args);
}

std::unique_ptr<ExprAST> Parser::parseNumberExpr(s_cursor_t &it, const s_cursor_t &end) {
    auto result = llvm::make_unique<NumberExprAST>(_number);
    readToken(it, end);
    return std::move(result);
}

std::unique_ptr<ExprAST> Parser::parseParenExpr(s_cursor_t &it, const s_cursor_t &end) {
    readToken(it, end); // eating current '('
    auto v = parseExpression(it, end);
    if (!v)
        return nullptr;
    readToken(it, end);
    return v;
}

std::unique_ptr<PrototypeAST> Parser::parseExtern(s_cursor_t &it, const s_cursor_t &end) {
    readToken(it, end);

    return parsePrototype(it, end);
}

/// prototype
///   ::= id '(' id* ')'
std::unique_ptr<PrototypeAST> Parser::parsePrototype(s_cursor_t &it, const s_cursor_t &end) {
    if (_lastTok != tok_identifier)
        return ErrorP("Expected function name in prototype");

    std::string fnName = _identifier;
    readToken(it, end);

    if (_lastTok != '(' )
        return ErrorP("Expected '(' in prototype");

    std::vector<std::string> argNames;
    while (readToken(it, end) == tok_identifier)
        argNames.push_back(_identifier);

    if (_lastTok != ')')
        return ErrorP("Expected ')' in prototype");

    readToken(it, end);

    return llvm::make_unique<PrototypeAST>(fnName, std::move(argNames));
}

std::unique_ptr<FunctionAST> Parser::parseTopLevelExpr(s_cursor_t &it, const s_cursor_t &end) {
    if (auto e = parseExpression(it, end)) {
        auto proto = llvm::make_unique<PrototypeAST>("__anon_expr",
                                                     std::vector<std::string>());
        return llvm::make_unique<FunctionAST>(std::move(proto), std::move(e));
    }

    return nullptr;
}

int Parser::readToken(s_cursor_t &it, const s_cursor_t &end) {
  _lastTok = ' '; // the last read char from input

  while (it != end && isspace(_lastTok)) {
    _lastTok = *(++it);
  }

  if (isalpha(_lastTok)) { /// identifier: [a-zA-Z][a-zA-Z0-9]*
    _identifier = _lastTok;
    while (it != end && isalnum((_lastTok = *(++it))))
      _identifier += _lastTok;

    if (_identifier == def_id)
      return tok_def;
    if (_identifier == extern_id)
      return tok_extern;
    return tok_identifier;
  }

  if (isdigit(_lastTok)) { // number [0-9]
    std::string num;
    do {
      num += _lastTok;
    } while (it != end && isdigit((_lastTok = *(++it))));
    _number = strtol(num.c_str(), nullptr, 10);
    
    return tok_number; 
  }

  if (_lastTok == '#') {
    // just a loop for ignoring everything after the #
    while (it != end && (_lastTok = *(++it)) != '\n')
      if (_lastTok != '\r' && _lastTok != EOF)
        continue;
  }

  if (it == end)
    return tok_eof;

  // don't know what to do with this char
  return _lastTok;
}

int Parser::currentTokPrecedence() {
    if (!isascii(_lastTok))
        return -1;

    int p = BIN_OP_PRECEDENCE.at(_lastTok);
    if (p <= 0)
        return -1;

    return p;
}
