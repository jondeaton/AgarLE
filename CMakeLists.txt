cmake_minimum_required(VERSION 2.8)
project(AgarLE CXX)
set(CMAKE_CXX_STANDARD 17)

# To configure for "release", then run cmake with:
# cmake -DCMAKE_BUILD_TYPE=Release

add_subdirectory(agario)
add_subdirectory(environment)