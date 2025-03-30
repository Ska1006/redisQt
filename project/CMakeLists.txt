cmake_minimum_required(VERSION 3.16)

project(redisQt LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(redisQt SHARED
    ./src/redisconnector.cpp
    ./src/redisdata.cpp
    ./src/redissubscriber.cpp
    ./include/redisconnector.h
    ./include/redisdata.h
    ./include/redissubscriber.h
)

target_link_libraries(redisQt PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_link_libraries(redisQt PRIVATE hiredis)

target_compile_definitions(redisQt PRIVATE REDISQT__LIBRARY)
