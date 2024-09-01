cmake_minimum_required(VERSION 3.5)

project(Password-Generator)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(${PROJECT_NAME} main.cpp mainwindow.cpp mainwindow.h)

target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
