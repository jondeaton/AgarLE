cmake_minimum_required(VERSION 2.8)
project(Agario CXX)
set(CMAKE_CXX_STANDARD 17)

set(DEBUG_FLAGS "-g -O0 -Wall -Wextra -pedantic")
set(RELEASE_FLAGS "-O3")
set(BENCH_FLAGS "-Ofast -fno-omit-frame-pointer")

# To configure for "release", then run cmake with:
# cmake -DCMAKE_BUILD_TYPE=Release

function(make_includable input_file output_file)
    file(READ ${input_file} content)
    set(delim "for_c++_include")
    set(content "R\"${delim}(\n${content})${delim}\"")
    file(WRITE ${output_file} "${content}")
endfunction(make_includable)

make_includable(rendering/shaders/vertex.glsl rendering/shaders/_vertex.glsl)
make_includable(rendering/shaders/fragment.glsl rendering/shaders/_fragment.glsl)


IF(APPLE)
    # Fix linking on 10.14+. See https://stackoverflow.com/questions/54068035
    link_directories(/usr/local/lib)
ENDIF()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Mode: Release")
    set(CMAKE_CXX_FLAGS  "${RELEASE_FLAGS}")
else()
    message(STATUS "Mode: Debug")
    set(CMAKE_CXX_FLAGS "${DEBUG_FLAGS}")
endif()

include_directories(".")

set(AGARIO_CORE_SRC
        core/core.hpp
        core/Player.hpp
        core/utils.hpp          core/utils.cpp
        core/Ball.hpp
        core/types.hpp
        core/num_wrapper.hpp
        core/Entities.hpp)

set(AGARIO_BOT_SRC
        bots/bots.hpp
        bots/RandomBot.hpp
        bots/HungryBot.hpp
        bots/HungryShyBot.hpp)

set(AGARIO_ENGINE_SRC
        ${AGARIO_BOT_SRC}
        engine/Engine.hpp
        engine/GameState.hpp
        core/settings.hpp)

set(AGARIO_RENDERING_SRC
        rendering/platform.hpp
        rendering/renderer.hpp
        rendering/shader.hpp
        rendering/window.hpp
        rendering/FrameBufferObject.hpp)

set(AGARIO_SRC ${AGARIO_CORE_SRC} ${AGARIO_ENGINE_SRC})

add_library(agario STATIC ${AGARIO_SRC})

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_library(GLFW REQUIRED)
include_directories(client engine ${OPENGL_INCLUDE_DIR})

set(AGARIO_CLIENT_SRC
        ${AGARIO_BOT_SRC}
        ${AGARIO_RENDERING_SRC}
        core/Entities.hpp
        client/client.hpp
        rendering/renderer.hpp
        rendering/shader.hpp)

add_executable(client ${AGARIO_CLIENT_SRC} ${AGARIO_BOT_SRC} client/main.cpp)
target_link_libraries(client agario gtest glfw glew ${OPENGL_LIBRARIES} ${OPENGL_glu_LIBRARY})

include_directories(server)
set(AGARIO_SERVER_SRC
        server/main.cpp)
add_executable(server ${AGARIO_SERVER_SRC})
target_link_libraries(server agario)

include_directories(test)
set(TEST_SRC
        test/test-engine.hpp
        test/main.cpp)

find_package(gtest)
if (GTEST_FOUND)
    #set(TEST_FLAGS "-g -O0 -Wall -Wextra -pedantic")

    include_directories(test ${GTEST_INCLUDE_DIRS})

    add_executable(test-engine ${TEST_SRC} ${AGARIO_SRC})
    target_link_libraries(test-engine pthread gtest)
#    target_compile_options(test-engine PUBLIC ${TEST_FLAGS})
else()
    message("Google Test not found")
endif()
