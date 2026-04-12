cmake_minimum_required(VERSION 3.10)
project(Ini)

add_library(Ini
        entities/Ini_parser.cpp
        entities/Ini_parser.hpp

        data/Constants.hpp
)
