cmake_minimum_required(VERSION 3.10)
project(Models)

add_library(Models
        entities/Search_hit_model.cpp
        entities/Search_hit_model.hpp
        entities/SearchHit.hpp
)
