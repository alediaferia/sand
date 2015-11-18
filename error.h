//
// Created by Alessandro Diaferia on 16/11/2015.
//
#include "printable.h"

class Error : public Printable {
public:
    Error(const std::string &message);
    Error(std::string &&message);
    Error(const char *message);
    virtual ~Error();

    void setData(void *data, bool takeOwnership = false);
    void *data() const;

    virtual std::string printable() const;

private:
    std::string _message;
    void *_data;
};
