# A minimal project that uses an *installed* oscpp through find_package(), to check the install and the exported target.
# It is not part of the main build. Try it with:
#   cmake --install <build dir> --prefix <prefix>
#   cmake -S test/package_consumer -B <consumer build dir> -DCMAKE_PREFIX_PATH=<prefix>
#   cmake --build <consumer build dir>
#   <consumer build dir>/package_consumer
cmake_minimum_required(VERSION 3.25)
project(package_consumer LANGUAGES CXX)

find_package(oscpp REQUIRED)

add_executable(package_consumer main.cpp)
target_link_libraries(package_consumer PRIVATE oscpp::oscpp)
