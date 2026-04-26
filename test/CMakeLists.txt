project(oscpp_test)
cmake_minimum_required(VERSION 3.25)

set (Boost_USE_STATIC_LIBS OFF)
find_package (Boost REQUIRED COMPONENTS unit_test_framework CONFIG)

add_executable(test_circular_buffer test_circular_buffer.cpp)
target_include_directories(test_circular_buffer PRIVATE "../include" "$ENV{HOME}/Projects")
target_link_libraries(test_circular_buffer PRIVATE oscpp)
add_test(NAME test_circular_buffer COMMAND test_circular_buffer)

add_executable(test_dynamic_library test_dynamic_library.cpp)
target_include_directories(test_dynamic_library PRIVATE "../include" "${Boost_INCLUDE_DIRS}")
target_link_libraries(test_dynamic_library PRIVATE oscpp "${Boost_LIBRARIES}")
add_test(NAME test_dynamic_library COMMAND test_dynamic_library)

add_executable(test_file test_file.cpp)
target_include_directories(test_file PRIVATE "../include" "${Boost_INCLUDE_DIRS}")
target_link_libraries(test_file PRIVATE oscpp "${Boost_LIBRARIES}")
add_test(test_file COMMAND test_file)

add_executable(test_file_descriptor test_file_descriptor.cpp)
target_include_directories(test_file_descriptor PRIVATE "../include" "${Boost_INCLUDE_DIRS}")
target_link_libraries(test_file_descriptor PRIVATE oscpp "${Boost_LIBRARIES}")
add_test(test_file_descriptor COMMAND test_file_descriptor)

add_executable(test_random_device test_random_device.cpp)
target_include_directories(test_random_device PRIVATE "../include" "${Boost_INCLUDE_DIRS}")
target_link_libraries(test_random_device PRIVATE oscpp "${Boost_LIBRARIES}")
add_test(test_random_device COMMAND test_random_device)

add_executable(test_socket test_socket.cpp)
target_include_directories(test_socket PRIVATE "../include" "${Boost_INCLUDE_DIRS}")
target_link_libraries(test_socket PRIVATE oscpp "${Boost_LIBRARIES}")
add_test(NAME test_socket COMMAND test_socket)

add_executable(test_stopwatch test_stopwatch.cpp)
target_include_directories(test_stopwatch PRIVATE "../include" "${Boost_INCLUDE_DIRS}")
target_link_libraries(test_stopwatch PRIVATE oscpp "${Boost_LIBRARIES}")
add_test(NAME test_stopwatch COMMAND test_stopwatch)

add_executable(test_system_exception test_system_exception.cpp)
target_include_directories(test_system_exception PRIVATE "../include" "${Boost_INCLUDE_DIRS}")
target_link_libraries(test_system_exception PRIVATE oscpp "${Boost_LIBRARIES}")
add_test(NAME test_system_exception COMMAND test_system_exception)

add_executable(test_trim test_trim.cpp)
target_include_directories(test_trim PRIVATE "../include" "${Boost_INCLUDE_DIRS}")
target_link_libraries(test_trim PRIVATE oscpp "${Boost_LIBRARIES}")
add_test(NAME test_trim COMMAND test_trim)

include(CTest)
