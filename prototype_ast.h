#ifndef PROTOTYPE_AST_H
#define PROTOTYPE_AST_H

#include <vector>
#include <string>

class PrototypeAST {
public:
  PrototypeAST(const std::string &name,
      std::vector<std::string> args);

private:
  std::string _name;
  std::vector<std::string> _args;
};

#endif // PROTOTYPE_AST_H
