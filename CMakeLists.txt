cmake_minimum_required(VERSION 3.31)
project(Search_system)

add_compile_options(-Wall -Wextra)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_DEFAULT_BUILD_TYPE "Debug" CACHE STRING "" FORCE)

message("Available configs: ${CMAKE_CONFIGURATION_TYPES}")
message("Default config: ${CMAKE_DEFAULT_BUILD_TYPE}")
find_package(Qt6 COMPONENTS
        Core
        Widgets
        REQUIRED
)

add_executable(Search_system
        main.cpp

        core/entities/Indexer/entities/Indexer.cpp
        core/entities/Indexer/entities/Indexer.hpp

        core/entities/Database/entities/DbController.cpp
        core/entities/Database/entities/DbController.hpp
        core/entities/Database/entities/SQLexception.hpp

        core/entities/Ini_parser/entities/Ini_parser.cpp
        core/entities/Ini_parser/entities/Ini_parser.hpp
        core/entities/Ini_parser/data/Constants.hpp

        core/entities/Model/entities/Search_hit_model.cpp
        core/entities/Model/entities/Search_hit_model.hpp
        core/entities/Model/entities/SearchHit.hpp

        core/entities/UI/Interface.cpp
        core/entities/UI/Interface.hpp

        libio.cppm
)

if (WIN32)
    #TODO change path in windows
    add_subdirectory(C:/path/to/libpqxx libpqxx-build)
endif ()

target_link_libraries(Search_system
        PRIVATE
        Qt6::Core
        Qt6::Widgets
        pqxx
)

target_sources(Search_system
        PUBLIC
        FILE_SET all_my_modules TYPE CXX_MODULES FILES
        libio.cppm
)
