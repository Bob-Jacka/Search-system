cmake_minimum_required(VERSION 3.31)
project(Ini)

add_library(Ini
        entities/Ini_parser.cpp
        entities/Ini_parser.hpp

        data/Constants.hpp
)

target_link_libraries(Ini Libio)
