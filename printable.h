//
// Created by Alessandro Diaferia on 16/11/2015.
//
#ifndef PRINTABLE_H
#define PRINTABLE_H
#include <string>

const size_t MAX_PRINTABLE_BUFSIZE = 1024;

class Printable {
public:
    virtual std::string printable() const = 0;
};
typedef std::shared_ptr<Printable> PrintableRef;

#endif
