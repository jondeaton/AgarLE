cmake_minimum_required(VERSION 2.8)
project(Agario CXX)
set(CMAKE_CXX_STANDARD 17)

#set(DEBUG_FLAGS -g -O0 -Wall -Wextra -pedantic -DDEBUG)
#set(RELEASE_FLAGS -Ofast -DNDEBUG)
#set(BENCH_FLAGS -Ofast -fno-omit-frame-pointer -DNDEBUG)
set(TEST_FLAGS -g -O0 -Wall -Wextra -pedantic -DDEBUG)

# To configure for "release", then run cmake with:
# cmake -DCMAKE_BUILD_TYPE=Release
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Mode: Release")
    set(CMAKE_CXX_FLAGS  ${RELEASE_FLAGS})
else()
    message(STATUS "Mode: Debug")
    set(CMAKE_CXX_FLAGS ${DEBUG_FLAGS})
endif()

include_directories(engine engine/THST)
set(AGARIO_SRC
        engine/Engine.hpp       engine/Engine.cpp
        engine/Player.hpp
        engine/utils.hpp        engine/utils.cpp
        engine/Ball.hpp
        engine/settings.hpp
        engine/Entities.hpp)
#engine/THST/QuadTree.h
add_library(agario STATIC ${AGARIO_SRC})

include_directories(client)
set(AGARIO_CLIENT_SRC
        client/main.cpp)
add_executable(client ${AGARIO_CLIENT_SRC})
target_link_libraries(client agario)

include_directories(server)
set(AGARIO_SERVER_SRC
        server/main.cpp)
add_executable(server ${AGARIO_SERVER_SRC})
target_link_libraries(server agario)


include_directories(test)
set(TEST_SRC
        test/engine-test.hpp
        test/main.cpp)

find_library(gtest gtest)
if (gtest)
    include_directories(test ${GTEST_INCLUDE_DIR})

    add_executable(engine-test ${TEST_SRC} ${AGARIO_SRC})
    target_link_libraries(engine-test gtest pthread)
    target_compile_options(engine-test PUBLIC ${TEST_FLAGS})
else()
    message("Google Test not found")
endif()