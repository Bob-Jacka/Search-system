cmake_minimum_required(VERSION 3.31)
project(Search_system)

add_compile_options(-Wall -Wextra)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_DEFAULT_BUILD_TYPE "Debug" CACHE STRING "" FORCE)

message("Available configs: ${CMAKE_CONFIGURATION_TYPES}")
message("Default config: ${CMAKE_DEFAULT_BUILD_TYPE}")
find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED
)
include_directories(${OpenCV_INCLUDE_DIRS})

add_subdirectory(core/entities)

add_executable(Search_system
        main.cpp
        search_system.ui
        libio.cppm
)

target_link_libraries(Search_system
        PRIVATE
        Qt6::Core
        Qt6::Widgets
)

target_sources(Search_system
        PUBLIC
        FILE_SET all_my_modules TYPE CXX_MODULES FILES
        libio.cppm
)
