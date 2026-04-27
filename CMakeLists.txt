cmake_minimum_required(VERSION 3.25)
project(oscpp VERSION 0.1.1 DESCRIPTION "Library for operating system like functions" LANGUAGES CXX)
enable_testing()

set(BINDIR bin)
set(LIBDIR lib)
set(INCLUDEDIR include)

set(CMAKE_CXX_STANDARD 23)

set(HEADERS include/circular_buffer.hpp
        include/dynamiclibrary.hpp
        include/file.hpp
        include/file_descriptor.hpp
        include/random_device.hpp
        include/socket.hpp
        include/stopwatch.hpp
        include/sysexception.hpp
        include/trim.hpp)

set(SOURCES src/circular_buffer.cpp
        src/circular_buffer.cpp
        src/dynamiclibary.cpp
        src/file.cpp
        src/file_descriptor.cpp
        src/random_device.cpp
        src/socket.cpp
        src/stopwatch.cpp
        src/sysexception.cpp
        src/trim.cpp)

add_library(oscpp SHARED ${HEADERS} ${SOURCES})
target_include_directories(oscpp PRIVATE ./include)
target_compile_options(oscpp PRIVATE -fvisibility=hidden)
set_target_properties(oscpp PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION 1)
set_target_properties(oscpp PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
set_target_properties(oscpp PROPERTIES PUBLIC_HEADER "${HEADERS}")
install(TARGETS ${PROJECT_NAME} EXPORT oscpp
        ARCHIVE  DESTINATION "${LIBDIR}"
        LIBRARY  DESTINATION "${LIBDIR}"
        RUNTIME  DESTINATION "${BINDIR}"
        PUBLIC_HEADER DESTINATION "${INCLUDEDIR}")
install(EXPORT oscpp DESTINATION share/oscpp/cmake)
export(TARGETS ${PROJECT_NAME} FILE oscpp.cmake)

add_subdirectory(test)

