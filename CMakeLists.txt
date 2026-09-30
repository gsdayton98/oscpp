cmake_minimum_required(VERSION 3.25)
project(oscpp VERSION 1.0.0 DESCRIPTION "Library for operating system like functions" LANGUAGES CXX)
enable_testing()

include(GNUInstallDirs)
# Override system installation when developing.
# For actual installation, set the values on the command line.
# This avoids needing privileged access during development.
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "$ENV{HOME}" CACHE PATH "Install prefix" FORCE)
endif()


set(BINDIR bin)
set(LIBDIR lib)
set(INCLUDEDIR include)

set(CMAKE_CXX_STANDARD 23)

set(HEADERS include/oscpp/circular_buffer.hpp
        include/oscpp/dynamiclibrary.hpp
        include/oscpp/file.hpp
        include/oscpp/file_descriptor.hpp
        include/oscpp/random_device.hpp
        include/oscpp/socket.hpp
        include/oscpp/stopwatch.hpp
        include/oscpp/sysexception.hpp
        include/oscpp/trim.hpp
        include/oscpp_exceptionless/file_descriptor.hpp
        include/oscpp_exceptionless/socket.hpp)

set(SOURCES src/oscpp/circular_buffer.cpp
        src/oscpp/dynamiclibrary.cpp
        src/oscpp/file.cpp
        src/oscpp/file_descriptor.cpp
        src/oscpp/random_device.cpp
        src/oscpp/socket.cpp
        src/oscpp/stopwatch.cpp
        src/oscpp/sysexception.cpp
        src/oscpp/trim.cpp
        src/oscpp_exceptionless/file_descriptor.cpp
        src/oscpp_exceptionless/socket.cpp)

add_library(oscpp SHARED ${HEADERS} ${SOURCES})
target_include_directories(oscpp
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_options(oscpp PRIVATE -fvisibility=hidden)
set_target_properties(oscpp PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(oscpp PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

install(TARGETS ${PROJECT_NAME} EXPORT oscpp
        ARCHIVE  DESTINATION "${LIBDIR}"
        LIBRARY  DESTINATION "${LIBDIR}"
        RUNTIME  DESTINATION "${BINDIR}"
        PUBLIC_HEADER DESTINATION "${INCLUDEDIR}")
# Preserve the oscpp/ and oscpp_exceptionless/ header directories.
install(DIRECTORY include/oscpp include/oscpp_exceptionless DESTINATION "${INCLUDEDIR}" FILES_MATCHING PATTERN "*.hpp")
install(EXPORT oscpp NAMESPACE oscpp:: DESTINATION "${LIBDIR}/cmake/oscpp")
export(TARGETS ${PROJECT_NAME} NAMESPACE oscpp:: FILE oscpp.cmake)

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