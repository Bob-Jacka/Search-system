#ifndef NETOLIBPQ_SQLEXCEPTION_HPP
#define NETOLIBPQ_SQLEXCEPTION_HPP

#include <exception>
#include <iostream>

class SQLexception final : std::exception {
public:
    explicit SQLexception(int line, const std::string &msg, const char *filename);
};

inline SQLexception::SQLexception(const int line, const std::string &msg, const char *filename) {
    std::cout << "Error: " << msg << " at line " << line << " at file " << filename << std::endl;
}

#endif
