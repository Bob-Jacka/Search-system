cmake_minimum_required(VERSION 3.10)
project(Ini)

add_library(Ini
        entities/Ini_parser.cpp
        entities/Ini_parser.hpp
        exceptions/ParserException.cpp
        exceptions/ParserException.hpp

        data/Constants.hpp
)
