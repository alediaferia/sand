//
// Created by Alessandro Diaferia on 16/11/2015.
//
#ifndef ERROR_H
#define ERROR_H

#include "printable.h"

class Error : public Printable {
public:
    Error(const std::string &message);
    Error(std::string &&message);
    virtual ~Error();

    //void setData(void *data, bool takeOwnership = false);
    //void *data() const;

    virtual std::string printable() const;

private:
    std::string _message;
    void *_data;
};
#endif
