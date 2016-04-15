//
// Created by Alessandro Diaferia on 16/11/2015.
//
#ifndef CONSOLE_H
#define CONSOLE_H

// includes //
#include <string>
#include <llvm/IR/Module.h>

using namespace llvm;

// forward declarations //
class Parser;

// class definition //
class Console {
public:
    Console();
    ~Console();

    Parser *parser() const;

    void run();
    void setPrompt(std::string &&prompt);
    std::string prompt() const;

protected:
    void readline(const std::string &prompt);

private:
    bool volatile _running;
    Parser *_parser;
    std::string _prompt;
    std::shared_ptr<Module> _module;

    friend void _int_handler(int);
};

#endif // CONSOLE_H
