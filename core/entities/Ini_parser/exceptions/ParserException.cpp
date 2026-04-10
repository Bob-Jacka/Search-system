#include "ParserException.hpp"

#include <iostream>

Parser_n::ParserException::ParserException(const std::string &msg, const std::source_location &location) {
    message = msg;
    std::cout << "An exception occurred in file " << location.file_name() << "\n";
    std::cout << "At line " << location.line() << "\n";
    std::cout << "In column " << location.column() << "\n";
    std::cout << "In function with name " << location.function_name() << "\n";
}

const char *Parser_n::ParserException::what() const noexcept {
    return message.c_str();
}
