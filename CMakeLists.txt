cmake_minimum_required(VERSION 3.25)
project(oscpp)

set(CMAKE_CXX_STANDARD 23)

include_directories(include)

set(HEADERS include/dynamiclibrary.hpp
        include/sysexception.hpp
        include/trim.hpp
        src/sysexception.cpp)

set(SOURCES src/sysexception.cpp  src/trim.cpp src/dynamiclibary.cpp)

add_library(oscpp SHARED ${HEADERS} ${SOURCES})

add_subdirectory(test)