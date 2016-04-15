#include "parser.h"

#include "number_expr_ast.h"
#include "var_expr_ast.h"
#include "call_expr_ast.h"
#include "binary_expr_ast.h"

#include <iostream>

static const char *SAND_STDIN_TAG = "<console>";

static const std::string def_id("def");
static const std::string extern_id("extern");

static const std::map<char, int> BIN_OP_PRECEDENCE = {
    {'<', 10},
    {'+', 20},
    {'-', 30},
    {'*', 40},
};

Parser::Parser(const std::shared_ptr<Module> &module) : _inputTag(nullptr),
    _builder(getGlobalContext()) {

    _lastChar = ' ';
    _module = module;
}

Parser::Parser(Type type, const std::shared_ptr<Module> &module) : Parser(module) {
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
}


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

    if (_curTok != '(')
        return llvm::make_unique<VarExprAST>(&_builder, idName, _symTable);

    getNextToken(it, end);

    std::vector<std::unique_ptr<ExprAST>> args;
    if (_curTok != ')') {
        while (true) {
            if (auto arg = parseExpression(it, end))
                args.push_back(std::move(arg));
            else
                return nullptr;

            if (_curTok == ')' || _curTok == tok_eof)
                break;

            if (_curTok != ',')
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
    getNextToken(it, end);
    while (_curTok == tok_identifier) {
        argNames.push_back(_identifier);
        if (getNextToken(it, end) == ',')
            getNextToken(it, end);
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

int Parser::getNextToken(s_cursor_t &b, const s_cursor_t &end, bool cleanBuffer) {
    if (cleanBuffer)
        _lastChar = ' ';
    return _curTok = readToken(b, end);
}

int Parser::currentTokPrecedence() const {
    if (!isascii(_curTok))
        return -1;

    auto it = BIN_OP_PRECEDENCE.find(_curTok);
    if (it == BIN_OP_PRECEDENCE.end()) {
        return -1;
    }
    return it->second;
}
