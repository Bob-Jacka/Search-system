cmake_minimum_required(VERSION 3.31)
project(Database)

add_library(Database
        entities/DbController.cpp
        entities/DbController.hpp

        entities/SQLexception.hpp
)

target_link_libraries(Database PUBLIC pqxx Qt6::Core Libio)
