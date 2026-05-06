cmake_minimum_required(VERSION 3.25)
project(oscpp VERSION 1.0.0 DESCRIPTION "Library for operating system like functions" LANGUAGES CXX)
enable_testing()

include(GNUInstallDirs)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "$ENV{HOME}" CACHE PATH "Install prefix" FORCE)
endif()


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
target_include_directories(oscpp
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_options(oscpp PRIVATE -fvisibility=hidden)
set_target_properties(oscpp PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION 1)
set_target_properties(oscpp PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
set_target_properties(oscpp PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS ${PROJECT_NAME} EXPORT oscpp
        ARCHIVE  DESTINATION "${LIBDIR}"
        LIBRARY  DESTINATION "${LIBDIR}"
        RUNTIME  DESTINATION "${BINDIR}"
        PUBLIC_HEADER DESTINATION "${INCLUDEDIR}")
install(EXPORT oscpp NAMESPACE oscpp:: DESTINATION "${LIBDIR}/cmake/oscpp")
export(TARGETS ${PROJECT_NAME} NAMESPACE oscpp FILE oscpp.cmake)

add_subdirectory(test)

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