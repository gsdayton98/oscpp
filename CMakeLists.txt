cmake_minimum_required(VERSION 3.25)
project(oscpp VERSION 0.1.1 DESCRIPTION "Library for operating system like functions" LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 23)

include_directories(include)

set(DEVROOT $ENV{HOME})

set(HEADERS include/dynamiclibrary.hpp
        include/sysexception.hpp
        include/trim.hpp
        src/sysexception.cpp)

set(SOURCES src/sysexception.cpp  src/trim.cpp src/dynamiclibary.cpp)

add_library(oscpp SHARED ${HEADERS} ${SOURCES})
target_include_directories(oscpp PRIVATE .)
set_target_properties(oscpp PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION 1)

install(TARGETS oscpp
        LIBRARY DESTINATION ${DEVROOT}/lib)
install(FILES ${HEADERS} DESTINATION ${DEVROOT}/include)
add_subdirectory(test)