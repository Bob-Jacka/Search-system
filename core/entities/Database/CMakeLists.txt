cmake_minimum_required(VERSION 3.10)
project(Database)

add_library(Database
        entities/DbController.cpp
        entities/DbController.hpp
        entities/SQLexception.hpp
)
