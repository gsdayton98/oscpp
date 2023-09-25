project(oscpp_test)
cmake_minimum_required(VERSION 3.25)

set (Boost_USE_STATIC_LIBS OFF)
find_package (Boost REQUIRED COMPONENTS unit_test_framework)

include_directories($ENV{HOME}/Projects)
include_directories (${Boost_INCLUDE_DIRS})

set(SOURCES test.cpp
        test_circular_buffer.cpp
        testDynamicLibrary.cpp
        testSystemException.cpp
        testTrim.cpp
        mockstrerror.cpp
        test_file_descriptor.cpp)

add_executable(test ${SOURCES})
target_link_libraries(test oscpp ${Boost_LIBRARIES})

add_test(NAME test COMMAND test)

include(CTest)
