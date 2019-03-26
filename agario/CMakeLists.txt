cmake_minimum_required(VERSION 2.8)
project(Agario CXX)
set(CMAKE_CXX_STANDARD 17)

#set(DEBUG_FLAGS "-g -O0 -Wall -Wextra -pedantic")
#set(RELEASE_FLAGS "-O3")
#set(BENCH_FLAGS "-Ofast -fno-omit-frame-pointer")
#set(TEST_FLAGS "-g -O0 -Wall -Wextra -pedantic")

# To configure for "release", then run cmake with:
# cmake -DCMAKE_BUILD_TYPE=Release

IF(APPLE)
    # Fix linking on 10.14+. See https://stackoverflow.com/questions/54068035
    link_directories(/usr/local/lib)
ENDIF()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Mode: Release")
    set(CMAKE_CXX_FLAGS  ${RELEASE_FLAGS})
else()
    message(STATUS "Mode: Debug")
    set(CMAKE_CXX_FLAGS ${DEBUG_FLAGS})
endif()

include_directories(".")

set(AGARIO_SRC
        engine/Engine.hpp       engine/Engine.cpp
        engine/Player.hpp       engine/Player.cpp
        engine/utils.hpp        engine/utils.cpp
        engine/Ball.hpp
        engine/settings.hpp
        engine/Entities.hpp)

add_library(agario STATIC ${AGARIO_SRC})


find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories(client engine ${OPENGL_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})

set(AGARIO_CLIENT_SRC
        engine/Entities.hpp
        client/client.hpp
        client/renderables.hpp
        client/renderer.hpp
        client/shader.hpp)

add_executable(test-opengl test/test-opengl.cpp)
target_link_libraries(test-opengl glfw ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${OPENGL_glu_LIBRARY})

add_executable(test-client ${AGARIO_CLIENT_SRC} test/test-client.cpp)
target_link_libraries(test-client agario glfw ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${OPENGL_glu_LIBRARY})

add_executable(client ${AGARIO_CLIENT_SRC} client/main.cpp)
target_link_libraries(client agario glfw ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${OPENGL_glu_LIBRARY})

include_directories(server)
set(AGARIO_SERVER_SRC
        server/main.cpp)
add_executable(server ${AGARIO_SERVER_SRC})
target_link_libraries(server agario)

include_directories(test)
set(TEST_SRC
        test/test-engine.hpp
        test/main.cpp)

find_package(gtest REQUIRED)
if (GTEST_FOUND)

    include_directories(test ${GTEST_INCLUDE_DIRS})

    add_executable(test-engine ${TEST_SRC} ${AGARIO_SRC})
    target_link_libraries(test-engine pthread gtest)
    target_compile_options(test-engine PUBLIC ${TEST_FLAGS})
else()
    message("Google Test not found")
endif()