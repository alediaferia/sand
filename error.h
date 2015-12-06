//
// Created by Alessandro Diaferia on 16/11/2015.
//
#ifndef ERROR_H
#define ERROR_H

#include "expr_ast.h"
#include <memory>

#include <llvm/IR/Value.h>

using namespace llvm;

std::unique_ptr<ExprAST> Error(const char *message);
Value* ErrorV(const char *message);

#endif
