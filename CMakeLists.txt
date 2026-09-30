cmake_minimum_required(VERSION 3.25)
project(oscpp VERSION 1.0.0 DESCRIPTION "Library for operating system like functions" LANGUAGES CXX)

# Build the tests only when oscpp is the top-level project, so consumers using FetchContent don't need Boost.
option(OSCPP_BUILD_TESTING "Build the oscpp tests (requires Boost.Test)" ${PROJECT_IS_TOP_LEVEL})

include(GNUInstallDirs)
# Override system installation when developing.
# For actual installation, set the values on the command line.
# This avoids needing privileged access during development.
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
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

set(HEADERS include/oscpp/circular_buffer.hpp
        include/oscpp/dynamiclibrary.hpp
        include/oscpp/file.hpp
        include/oscpp/file_descriptor.hpp
        include/oscpp/random_device.hpp
        include/oscpp/socket.hpp
        include/oscpp/stopwatch.hpp
        include/oscpp/sysexception.hpp
        include/oscpp/trim.hpp
        include/oscpp_exceptionless/dynamiclibrary.hpp
        include/oscpp_exceptionless/file.hpp
        include/oscpp_exceptionless/file_descriptor.hpp
        include/oscpp_exceptionless/random_device.hpp
        include/oscpp_exceptionless/socket.hpp)

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

add_library(oscpp SHARED ${HEADERS} ${SOURCES})
add_library(oscpp::oscpp ALIAS oscpp)
target_link_libraries(oscpp PRIVATE $<BUILD_INTERFACE:oscpp_warnings>)
target_include_directories(oscpp
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Consumers inherit the C++23 requirement.
target_compile_features(oscpp PUBLIC cxx_std_23)
# Everything is hidden unless marked __attribute__((visibility("default"))).
set_target_properties(oscpp PROPERTIES CXX_VISIBILITY_PRESET hidden VISIBILITY_INLINES_HIDDEN ON)
set_target_properties(oscpp PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(oscpp PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

install(TARGETS ${PROJECT_NAME} EXPORT oscpp-targets
        ARCHIVE  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME  DESTINATION "${CMAKE_INSTALL_BINDIR}")
# Preserve the oscpp/ and oscpp_exceptionless/ header directories.
install(DIRECTORY include/oscpp include/oscpp_exceptionless DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" FILES_MATCHING PATTERN "*.hpp")
install(EXPORT oscpp-targets NAMESPACE oscpp:: DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/oscpp")
export(TARGETS ${PROJECT_NAME} NAMESPACE oscpp:: FILE oscpp-targets.cmake)

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
        COMPATIBILITY AnyNewerVersion)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/oscpp-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/oscpp-config-version.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/oscpp"
)