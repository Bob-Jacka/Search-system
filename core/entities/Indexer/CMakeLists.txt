cmake_minimum_required(VERSION 3.31)
project(Indexer)

add_library(Indexer
        entities/Indexer.cpp
        entities/Indexer.hpp
)

target_link_libraries(Indexer PUBLIC Qt6::Core Libio)
