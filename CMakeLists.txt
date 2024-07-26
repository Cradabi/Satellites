cmake_minimum_required(VERSION 3.5)

project(untitled7 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Gui Widgets Network REQUIRED)


add_executable(untitled7
main.cpp
mainwindow.cpp
mainwindow.h
)

target_link_libraries(untitled7 PRIVATE Qt5::Widgets Qt5::Core Qt5::Gui Qt5::Network)
