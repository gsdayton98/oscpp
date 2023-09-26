
set (Boost_USE_STATIC_LIBS OFF)
find_package (Boost REQUIRED COMPONENTS unit_test_framework)

include_directories($ENV{HOME}/Projects)
include_directories (${Boost_INCLUDE_DIRS})

add_executable(test_circular_buffer test_circular_buffer.cpp)
target_link_libraries(test_circular_buffer oscpp)

add_executable(testDynamicLibrary testDynamicLibrary.cpp)
target_link_libraries(testDynamicLibrary oscpp ${Boost_LIBRARIES})

add_executable(testSystemException testSystemException.cpp mockstrerror.cpp)
target_link_libraries(testSystemException oscpp ${Boost_LIBRARIES})

add_executable(testTrim testTrim.cpp)
target_link_libraries(testTrim oscpp ${Boost_LIBRARIES})

add_executable(test_file_descriptor test_file_descriptor.cpp)
target_link_libraries(test_file_descriptor oscpp ${Boost_LIBRARIES})

add_executable(test_socket test_socket.cpp)
target_link_libraries(test_socket oscpp ${Boost_LIBRARIES})

add_test(NAME test_circular_buffer COMMAND test_circular_buffer)
add_test(NAME testDynamicLibrary COMMAND testDynamicLibrary)
add_test(NAME testSystemException COMMAND testSystemException)
add_test(NAME testTrim COMMAND testTrim)
add_test(NAME test_file_descriptor COMMAND test_file_descriptor)
add_test(NAME test_socket COMMAND test_socket)

include(CTest)
