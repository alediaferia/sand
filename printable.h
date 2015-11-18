//
// Created by Alessandro Diaferia on 16/11/2015.
//

static const size_t MAX_PRINTABLE_BUFSIZE = 1024;

class Printable {
public:
    virtual std::string printable() const = 0;
}
