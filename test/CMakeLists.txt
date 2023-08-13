project(oscpp_test)
cmake_minimum_required(VERSION 3.25)

include_directories($ENV{HOME}/Projects)

set(SOURCES test.cpp
        test_circular_buffer.cpp
        testDynamicLibrary.cpp
        testSystemException.cpp
        testTrim.cpp
        mockstrerror.cpp)

add_executable(test ${SOURCES})

target_link_libraries(test oscpp)
