set (Boost_USE_STATIC_LIBS OFF)
find_package (Boost REQUIRED COMPONENTS unit_test_framework CONFIG)

add_executable(oscpp_tests oscpp_tests.cpp
        test_circular_buffer.cpp
        test_dynamic_library.cpp
        test_exceptionless_dynamic_library.cpp
        test_exceptionless_file.cpp
        test_exceptionless_file_descriptor.cpp
        test_exceptionless_random_device.cpp
        test_exceptionless_socket.cpp
        test_file.cpp
        test_file_descriptor.cpp
        test_random_device.cpp
        test_socket.cpp
        test_system_exception.cpp
        test_stopwatch.cpp
        test_trim.cpp)
# Boost's own headers trip -Wconversion and -Wsign-compare, so mark them as system headers to check only oscpp's code.
# CMake drops implicit include directories such as /usr/local/include from the command line (and AppleClang doesn't treat
# that one as a system directory), so pass -isystem explicitly.
get_target_property(OSCPP_BOOST_INCLUDE_DIRS Boost::unit_test_framework INTERFACE_INCLUDE_DIRECTORIES)
set(OSCPP_BOOST_ISYSTEM "")
foreach(dir IN LISTS OSCPP_BOOST_INCLUDE_DIRS)
    list(APPEND OSCPP_BOOST_ISYSTEM "SHELL:-isystem ${dir}")
endforeach()
target_compile_options(oscpp_tests PRIVATE ${OSCPP_BOOST_ISYSTEM})
target_compile_definitions(oscpp_tests PRIVATE BOOST_TEST_DYN_LINK)
target_link_libraries(oscpp_tests PRIVATE oscpp oscpp_warnings Boost::unit_test_framework)
add_test(NAME boost_tests COMMAND oscpp_tests)

# Multithreaded stress tests are too slow to be unit tests. Run them on their own with `ctest -L stress`, or skip
# them with `ctest -LE stress`.
add_executable(stress_circular_buffer stress_circular_buffer.cpp)
target_compile_options(stress_circular_buffer PRIVATE ${OSCPP_BOOST_ISYSTEM})
target_compile_definitions(stress_circular_buffer PRIVATE BOOST_TEST_DYN_LINK)
target_link_libraries(stress_circular_buffer PRIVATE oscpp oscpp_warnings Boost::unit_test_framework)
add_test(NAME stress_circular_buffer COMMAND stress_circular_buffer)
set_tests_properties(stress_circular_buffer PROPERTIES LABELS stress)
