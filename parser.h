//
// Created by Alessandro Diaferia on 16/11/2015.
//

#ifndef PARSER_H
#define PARSER_H

#include "lval.h"
#include "either.h"
#include "error.h"

#include "function_ast.h"
#include "prototype_ast.h"
#include "expr_ast.h"
#include "symbol_table.h"

#include <memory>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
struct mpc_parser_t;
struct mpc_ast_t;

enum Token {
    tok_eof = -1,
    
    // commands
    tok_def = -2,
    tok_extern = -3,
    
    // primary
    tok_identifier = -4,
    tok_number = -5,
    
    // other
    tok_parenop = -6,
    tok_parencl = -7,
    tok_unfinished = -99
};

typedef std::string::const_iterator s_cursor_t;

class Parser {
public:
    enum Type {
        File,
        Stdin
    };
    Parser(Type type);
    ~Parser();
    
    // Either<LValRef, ErrorRef> parse(const std::string &input);

    /*
     * Reads the next token from a string iterator.
     */
    int getNextToken(s_cursor_t &b, const s_cursor_t &end);
    
    std::unique_ptr<FunctionAST> parseDefinition(s_cursor_t &it, const s_cursor_t &end);
    std::unique_ptr<PrototypeAST> parsePrototype(s_cursor_t &it, const s_cursor_t &end);
    std::unique_ptr<FunctionAST> parseTopLevelExpr(s_cursor_t &it, const s_cursor_t &end);
    std::unique_ptr<ExprAST> parseExpression(s_cursor_t &it, const s_cursor_t &end);
    std::unique_ptr<ExprAST> parseIdentifierExpr(s_cursor_t &it, const s_cursor_t &end);
    std::unique_ptr<ExprAST> parseNumberExpr(s_cursor_t &it, const s_cursor_t &end);
    std::unique_ptr<ExprAST> parseParenExpr(s_cursor_t &it, const s_cursor_t &end);
    std::unique_ptr<PrototypeAST> parseExtern(s_cursor_t &it, const s_cursor_t &end);
    std::unique_ptr<ExprAST> parseBinOpRHS(int prec, std::unique_ptr<ExprAST> rhs,
                                           s_cursor_t &it, const s_cursor_t &en);

    /// primary
    ///   ::= identifierexpr
    ///   ::= numberexpr
    ///   ::= parenexpr
    std::unique_ptr<ExprAST> parsePrimary(s_cursor_t &it, const s_cursor_t &end);
    
    const char* currentInputTag() const;
protected:
    Parser();

    int readToken(s_cursor_t &b, const s_cursor_t &end);

private:
    int currentTokPrecedence();
    
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
    int _lastChar;
    int _curTok;
    std::shared_ptr<Module> _module;
    IRBuilder<> _builder;
    SymbolTable _symTable;
};

#endif //PARSER_H
