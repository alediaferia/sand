#include "error.h"

#include <utility>

Error::Error(const std::string &message) : _message(message)
{}

Error::Error(std::string &&message) : _message(std::move(message)) {}

Error::~Error() {}

std::string Error::printable() const {
    return _message;
}
