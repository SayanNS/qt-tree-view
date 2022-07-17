cmake_minimum_required(VERSION 3.19)
project(Mikran)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIR})

add_executable(${PROJECT_NAME} main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        DatabaseModel.cpp
        Database.cpp
        CacheModel.cpp
        Cache.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets ${Boost_LIBRARIES})
