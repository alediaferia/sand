#include "parser.h"
#include "mpc.h"

#include "number_expr_ast.h"
#include "var_expr_ast.h"
#include "call_expr_ast.h"
#include "binary_expr_ast.h"
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

static const char *SAND_MODULE = "sand";

static const char *SAND_STDIN_TAG = "<console>";

static const std::string def_id("def");
static const std::string extern_id("extern");

static const std::map<char, int> BIN_OP_PRECEDENCE = {
    {'<', 10},
    {'+', 20},
    {'-', 30},
    {'*', 40},
};

Parser::Parser() : _inputTag(nullptr),
    _builder(getGlobalContext()) {

    _tags.number = mpc_new("number");
    _tags.symbol = mpc_new("symbol");
    _tags.sexpr  = mpc_new("sexpr");
    _tags.qexpr  = mpc_new("qexpr");
    _tags.expr   = mpc_new("expr");
    _tags.lispy  = mpc_new("lispy");

    mpca_lang(MPCA_LANG_DEFAULT,
    SAND_GRAMMAR,
    _tags.number, _tags.symbol, _tags.sexpr, _tags.qexpr, _tags.expr, _tags.lispy);
    _lastChar = ' ';
    _module = std::shared_ptr<Module>(new Module(SAND_MODULE, getGlobalContext()));
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
    getNextToken(it, end);

    auto proto = parsePrototype(it, end);
    if (!proto)
        return nullptr;

    if (auto e = parseExpression(it, end))
        return llvm::make_unique<FunctionAST>(&_builder, _module, std::move(proto), std::move(e), _symTable);

    return nullptr; // no definition so far
}

std::unique_ptr<ExprAST> Parser::parseExpression(s_cursor_t &it, const s_cursor_t &end) {
    auto lhs = parsePrimary(it, end);
    if (!lhs)
        return nullptr;

    return parseBinOpRHS(0, std::move(lhs), it, end);
}

std::unique_ptr<ExprAST> Parser::parseBinOpRHS(int prec,
                                               std::unique_ptr<ExprAST> lhs,
                                               s_cursor_t &it, const s_cursor_t &end) {
    while (true) {
        int prec_ = currentTokPrecedence();

        if (prec_ < prec) {
            return lhs;
        }

        int binOp = _curTok;
        getNextToken(it, end);

        auto rhs = parsePrimary(it, end);

        if (!rhs)
            return nullptr;

        int nextPrec = currentTokPrecedence();
        if (prec_ < nextPrec) {
            rhs = parseBinOpRHS(prec_ + 1, std::move(rhs), it, end);

            if (!rhs)
                return nullptr;
        }

        lhs = llvm::make_unique<BinaryExprAST>(&_builder, binOp, std::move(lhs), std::move(rhs));
    }
}

std::unique_ptr<ExprAST> Parser::parsePrimary(s_cursor_t &it, const s_cursor_t &end) {
    switch (_curTok) {
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
    getNextToken(it, end);

    if (_lastChar != '(')
        return llvm::make_unique<VarExprAST>(&_builder, idName, _symTable);

    getNextToken(it, end);

    std::vector<std::unique_ptr<ExprAST>> args;
    if (_lastChar != ')') {
        while (true) {
            if (auto arg = parseExpression(it, end))
                args.push_back(std::move(arg));
            else
                return nullptr;

            if (_lastChar == ')')
                break;

            if (_lastChar != ',')
                return Error("Expected ')' or ',' in argument list");

            getNextToken(it, end);
        }
    }

    getNextToken(it, end); // eat ')'

    return llvm::make_unique<CallExprAST>(&_builder, _module, idName, std::move(args));
}

std::unique_ptr<ExprAST> Parser::parseNumberExpr(s_cursor_t &it, const s_cursor_t &end) {
    auto result = llvm::make_unique<NumberExprAST>(_number);
    getNextToken(it, end);
    return std::move(result);
}

std::unique_ptr<ExprAST> Parser::parseParenExpr(s_cursor_t &it, const s_cursor_t &end) {
    getNextToken(it, end); // eating current '('
    auto v = parseExpression(it, end);
    if (!v)
        return nullptr;
    getNextToken(it, end);
    return v;
}

std::unique_ptr<PrototypeAST> Parser::parseExtern(s_cursor_t &it, const s_cursor_t &end) {
    getNextToken(it, end);

    return parsePrototype(it, end);
}

/// prototype
///   ::= id '(' id* ')'
std::unique_ptr<PrototypeAST> Parser::parsePrototype(s_cursor_t &it, const s_cursor_t &end) {
    if (_curTok != tok_identifier)
        return ErrorP("Expected function name in prototype");

    std::string fnName = _identifier;
    getNextToken(it, end);

    if ( _curTok != '(' )
        return ErrorP("Expected '(' in prototype");

    std::vector<std::string> argNames;
    while (getNextToken(it, end) == tok_identifier) {
        argNames.push_back(_identifier);
    }

    if ( _curTok != ')' )
        return ErrorP("Expected ')' in prototype");

    getNextToken(it, end); // eat ')'

    return llvm::make_unique<PrototypeAST>(&_builder, _module, fnName, std::move(argNames));
}

std::unique_ptr<FunctionAST> Parser::parseTopLevelExpr(s_cursor_t &it, const s_cursor_t &end) {
    if (auto e = parseExpression(it, end)) {
        auto proto = llvm::make_unique<PrototypeAST>(&_builder, _module, "__anon_expr",
                                                     std::vector<std::string>());
        return llvm::make_unique<FunctionAST>(&_builder,
                                              _module,
                                              std::move(proto),
                                              std::move(e),
                                              _symTable);
    }

    return nullptr;
}

int Parser::readToken(s_cursor_t &it, const s_cursor_t &end) {
    // skipping any whitespace
    while (it != end && isspace(_lastChar))
      _lastChar = *(it++); // getting char and advancing cursor

    if (isalpha(_lastChar)) { /// identifier: [a-zA-Z][a-zA-Z0-9]*
        _identifier = _lastChar;
        while (it != end && isalnum((_lastChar = *(it++))))
          _identifier += _lastChar;

        if (_identifier == def_id)
            return tok_def;
        else if (_identifier == extern_id)
            return tok_extern;

        return tok_identifier;
    }

    if (isdigit(_lastChar)) { // number [0-9]
        std::string num;
        do {
          num += _lastChar;
        } while (it != end && isdigit((_lastChar = *(it++))));
        _number = strtol(num.c_str(), nullptr, 10);

        return tok_number;
    }

    if (_lastChar == '#') {
        // just a loop for ignoring everything after the #
        do
            _lastChar = *(it++);
        while (it != end && _lastChar != '\n' && _lastChar != '\r');

        if (_lastChar != EOF)
        return readToken(it, end);
    }

    if (it == end || _lastChar == EOF) {
      _lastChar = tok_eof;
    }

    // don't know what to do with this char
    int thisChar = _lastChar;
    _lastChar = *(it++);
    return thisChar;
}

int Parser::getNextToken(s_cursor_t &b, const s_cursor_t &end) {
    return _curTok = readToken(b, end);
}

int Parser::currentTokPrecedence() {
    if (!isascii(_curTok))
        return -1;

    int p = BIN_OP_PRECEDENCE.at(_curTok);
    if (p <= 0)
        return -1;

    return p;
}
