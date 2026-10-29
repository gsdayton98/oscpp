cmake_minimum_required(VERSION 3.25)
project(oscpp VERSION 2.0.0 DESCRIPTION "Library for operating system like functions" LANGUAGES CXX)

# Build the tests only when oscpp is the top-level project, so consumers using FetchContent don't need Boost.
option(OSCPP_BUILD_TESTING "Build the oscpp tests (requires Boost.Test)" ${PROJECT_IS_TOP_LEVEL})

include(GNUInstallDirs)
# The presets choose the prefix: `debug` installs to $HOME for development, `release` to /usr/local for delivery.
# Without a preset, a top-level build still defaults to $HOME so a plain install needs no privileges. Pass --prefix
# or -DCMAKE_INSTALL_PREFIX= to choose another. Never override the prefix of a parent project that embeds oscpp.
if(PROJECT_IS_TOP_LEVEL AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "$ENV{HOME}" CACHE PATH "Install prefix" FORCE)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Sanitizers for development builds. Set OSCPP_SANITIZE to a semicolon-separated list of sanitizers, for example
#   -DOSCPP_SANITIZE=address\;undefined      or      -DOSCPP_SANITIZE=thread
# The flags apply to the library, the tests and the stress tests. "thread" can't be combined with "address".
set(OSCPP_SANITIZE "" CACHE STRING "Sanitizers to enable (address, undefined, thread), semicolon separated")
if(OSCPP_SANITIZE)
    foreach(sanitizer IN LISTS OSCPP_SANITIZE)
        if(NOT sanitizer MATCHES "^(address|undefined|thread)$")
            message(FATAL_ERROR "OSCPP_SANITIZE: unknown sanitizer '${sanitizer}' (use address, undefined or thread)")
        endif()
    endforeach()
    if("thread" IN_LIST OSCPP_SANITIZE AND "address" IN_LIST OSCPP_SANITIZE)
        message(FATAL_ERROR "OSCPP_SANITIZE: 'thread' can't be combined with 'address'")
    endif()
    list(JOIN OSCPP_SANITIZE "," oscpp_sanitize_flags)
    message(STATUS "Sanitizers enabled: ${oscpp_sanitize_flags}")
    add_compile_options(-fsanitize=${oscpp_sanitize_flags} -fno-omit-frame-pointer -g)
    add_link_options(-fsanitize=${oscpp_sanitize_flags})
endif()

# Warnings used to build oscpp's own sources and tests. Not propagated to consumers.
option(OSCPP_WERROR "Treat warnings in oscpp's own code as errors" OFF)
add_library(oscpp_warnings INTERFACE)
target_compile_options(oscpp_warnings INTERFACE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
if(OSCPP_WERROR)
    target_compile_options(oscpp_warnings INTERFACE -Werror)
endif()

set(SOURCES src/oscpp/dynamiclibrary.cpp
        src/oscpp/file.cpp
        src/oscpp/file_descriptor.cpp
        src/oscpp/random_device.cpp
        src/oscpp/socket.cpp
        src/oscpp/stopwatch.cpp
        src/oscpp/sysexception.cpp
        src/oscpp/trim.cpp
        src/oscpp_exceptionless/dynamiclibrary.cpp
        src/oscpp_exceptionless/file.cpp
        src/oscpp_exceptionless/file_descriptor.cpp
        src/oscpp_exceptionless/random_device.cpp
        src/oscpp_exceptionless/socket.cpp)

add_library(oscpp SHARED ${SOURCES})
add_library(oscpp::oscpp ALIAS oscpp)
target_link_libraries(oscpp PRIVATE $<BUILD_INTERFACE:oscpp_warnings>)
# The public headers. install(TARGETS) installs the file set below the include directory, keeping the oscpp/ and
# oscpp_exceptionless/ directories. List new headers here so they are installed.
target_sources(oscpp PUBLIC FILE_SET HEADERS BASE_DIRS include FILES
        include/oscpp/circular_buffer.hpp
        include/oscpp/dynamiclibrary.hpp
        include/oscpp/file.hpp
        include/oscpp/file_descriptor.hpp
        include/oscpp/oscpp_export.hpp
        include/oscpp/random_device.hpp
        include/oscpp/socket.hpp
        include/oscpp/stopwatch.hpp
        include/oscpp/sysexception.hpp
        include/oscpp/thousands_separator.hpp
        include/oscpp/trim.hpp
        include/oscpp_exceptionless/dynamiclibrary.hpp
        include/oscpp_exceptionless/file.hpp
        include/oscpp_exceptionless/file_descriptor.hpp
        include/oscpp_exceptionless/random_device.hpp
        include/oscpp_exceptionless/socket.hpp)
target_include_directories(oscpp
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Consumers inherit the C++23 requirement.
target_compile_features(oscpp PUBLIC cxx_std_23)
# Everything is hidden unless marked OSCPP_API (include/oscpp_export.hpp).
set_target_properties(oscpp PROPERTIES CXX_VISIBILITY_PRESET hidden VISIBILITY_INLINES_HIDDEN ON)
target_compile_definitions(oscpp PRIVATE OSCPP_EXPORTS)
set_target_properties(oscpp PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(oscpp PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

# On macOS the install name is @rpath/liboscpp.N.dylib, which executables outside the install prefix can't resolve
# without an rpath. For delivery, the `release` preset records the absolute library path instead. The library then
# must be installed at the configured prefix, so don't combine this with `cmake --install --prefix`.
option(OSCPP_ABSOLUTE_INSTALL_NAME "Record the absolute install path as the library's install name (macOS)" OFF)
if(OSCPP_ABSOLUTE_INSTALL_NAME)
    set_target_properties(oscpp PROPERTIES INSTALL_NAME_DIR "${CMAKE_INSTALL_FULL_LIBDIR}")
endif()

install(TARGETS ${PROJECT_NAME} EXPORT oscpp-targets
        ARCHIVE  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME  DESTINATION "${CMAKE_INSTALL_BINDIR}"
        FILE_SET HEADERS DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(EXPORT oscpp-targets NAMESPACE oscpp:: DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/oscpp")
export(TARGETS ${PROJECT_NAME} NAMESPACE oscpp:: FILE oscpp-targets.cmake)

# Formatting: `cmake --build <dir> --target format` reformats the sources with .clang-format, and
# `--target check-format` fails if any file isn't already formatted. Set OSCPP_CLANG_FORMAT to choose the executable.
find_program(OSCPP_CLANG_FORMAT clang-format)
if(OSCPP_CLANG_FORMAT AND PROJECT_IS_TOP_LEVEL)
    file(GLOB_RECURSE OSCPP_FORMAT_FILES CONFIGURE_DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/test/*.hpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")
    add_custom_target(format
            COMMAND ${OSCPP_CLANG_FORMAT} -i ${OSCPP_FORMAT_FILES}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Formatting sources with clang-format"
            VERBATIM)
    add_custom_target(check-format
            COMMAND ${OSCPP_CLANG_FORMAT} --dry-run -Werror ${OSCPP_FORMAT_FILES}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Checking source formatting with clang-format"
            VERBATIM)
endif()

if(OSCPP_BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()

# Packaging support
set(CPACK_PACKAGE_VENDOR "Glen Dayton")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Operating System C++ Wrappers")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
include(CPack)

include(CMakePackageConfigHelpers)
configure_package_config_file(
        "cmake/oscpp-config.cmake.in"
        "oscpp-config.cmake"
        INSTALL_DESTINATION
        "${CMAKE_INSTALL_LIBDIR}/cmake/oscpp")

write_basic_package_version_file(
        "oscpp-config-version.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/oscpp-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/oscpp-config-version.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/oscpp"
)

# pkg-config file for consumers that don't use CMake.
configure_file("cmake/oscpp.pc.in" "oscpp.pc" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/oscpp.pc"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
