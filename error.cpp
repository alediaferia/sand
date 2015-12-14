#include "error.h"

std::unique_ptr<ExprAST> Error(const char *message) {
    fprintf(stderr, "Error: %s\n", message);
    return nullptr;
}

Value* ErrorV(const char *message) {
    Error(message);
    return nullptr;
}

std::unique_ptr<PrototypeAST> ErrorP(const char *message) {
    Error(message);
    return nullptr;
}
