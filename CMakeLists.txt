cmake_minimum_required(VERSION 2.8)
project(AgarLE CXX)
set(CMAKE_CXX_STANDARD 17)

set(DEBUG_FLAGS "-g -O0 -Wall -Wextra -pedantic")
set(RELEASE_FLAGS "-O3")
set(BENCH_FLAGS "-Ofast -fno-omit-frame-pointer")

# To configure for "release", then run cmake with:
# cmake -DCMAKE_BUILD_TYPE=Release

add_subdirectory(agario)
add_subdirectory(environment)