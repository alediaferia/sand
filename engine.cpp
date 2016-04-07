#include "engine.h"

Engine::Engine() {}

Value* Engine::eval(std::unique_ptr<FunctionAST> fast) {
    return fast->codegen();
}

Value* Engine::eval(std::unique_ptr<ExprAST> expr) {
    return expr->codegen();
}

