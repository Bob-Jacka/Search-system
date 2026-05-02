cmake_minimum_required(VERSION 3.31)
project(Search_system)

add_compile_options(-g -Wall -Wextra)

set(CMAKE_CXX_STANDARD 23)

add_executable(Search_system
        main.cpp

        core/entities/Indexer/entities/Indexer.cpp
        core/entities/Indexer/entities/Indexer.hpp

        core/entities/Database/entities/DbController.cpp
        core/entities/Database/entities/DbController.hpp

        core/entities/Ini_parser/entities/Ini_parser.cpp
        core/entities/Ini_parser/entities/Ini_parser.hpp
        core/entities/Ini_parser/data/Constants.hpp

        core/entities/UI/entities/SearchHit.hpp

        libio.cppm
)

if (WIN32)
    #TODO change path in windows system
    add_subdirectory(C:/path/to/libpqxx libpqxx-build)
endif ()

target_link_libraries(Search_system
        PRIVATE
        pqxx
)

target_sources(Search_system
        PUBLIC
        FILE_SET all_my_modules TYPE CXX_MODULES FILES
        libio.cppm
)
