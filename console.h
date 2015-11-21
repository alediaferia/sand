//
// Created by Alessandro Diaferia on 16/11/2015.
//
#ifndef CONSOLE_H
#define CONSOLE_H

#include <string>
class Parser;

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

    friend void _int_handler(int);
};
#endif
