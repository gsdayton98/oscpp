project(oscpp_test)
cmake_minimum_required(VERSION 3.25)

set (Boost_USE_STATIC_LIBS OFF)
find_package (Boost REQUIRED COMPONENTS unit_test_framework CONFIG)

add_executable(oscpp_tests oscpp_tests.cpp
        test_circular_buffer.cpp
        test_dynamic_library.cpp
        test_file.cpp
        test_file_descriptor.cpp
        test_random_device.cpp
        test_socket.cpp
        test_system_exception.cpp
        test_stopwatch.cpp
        test_trim.cpp)
target_include_directories(oscpp_tests PRIVATE "../include")
target_compile_definitions(oscpp_tests PRIVATE BOOST_TEST_DYN_LINK)
target_link_libraries(oscpp_tests PRIVATE oscpp Boost::unit_test_framework)
add_test(NAME boost_tests COMMAND oscpp_tests)

