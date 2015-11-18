//
// Created by Alessandro Diaferia on 16/11/2015.
//

#ifndef PARSER_H
#define PARSER_H

#include "lval.h"

class Parser {
public:
  enum Type {
    File,
    Stdin
  }
  Parser(Type type);
  ~Parser();

  void init();
private:
  Type _type;
};

#endif //PARSER_H
