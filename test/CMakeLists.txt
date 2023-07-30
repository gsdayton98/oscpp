project(oscpp_test)


include_directories(mockstrerror $ENV{HOME}/Projects)

set(SOURCES test.cpp
        testDynamicLibrary.cpp
        testSystemException.cpp
        testTrim.cpp)

add_executable(test ${SOURCES})

target_link_libraries(test oscpp)

add_subdirectory(mockstrerror)