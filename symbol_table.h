#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <map>
#include <llvm/IR/Value.h>

using namespace llvm;

/*class SymbolTable {
private:
    std::map<std::string, Value *> _map;
};*/

typedef std::map<std::string, Value*> SymbolTable;

#endif // SYMBOL_TABLE_H