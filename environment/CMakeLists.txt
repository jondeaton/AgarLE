cmake_minimum_required(VERSION 3.5)
project (AvarioEnv)

include_directories(".")
add_subdirectory(pybind11)

option(DEFINE_RENDERABLE "Renderable" OFF) # Enabled by default
if(DEFINE_RENDERABLE)
    message("Renderable")
    add_definitions(-DRENDERABLE)
endif(DEFINE_RENDERABLE)

set(AGARIO_FULL_ENV_SROUCE
        full_env_bindings.cpp
        envs/BaseEnvironment.hpp
        envs/FullEnvironment.hpp)

pybind11_add_module(agario_full_env ${AGARIO_FULL_ENV_SROUCE})
target_include_directories(agario_full_env PRIVATE "../agario")

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
if (OpenGL_FOUND)

    find_library(GLFW REQUIRED)
    find_package(glfw3)
    find_package(glm REQUIRED)

    include_directories(${GLM_INCLUDE_DIRS})
    link_directories(${GLM_LIBRARY_DIRS})
    add_definitions(${GLM_DEFINITIONS})

    include_directories(${OPENGL_INCLUDE_DIR} ${GLFW_INCLUDE_DIR})

    set(AGARIO_SCREEN_ENV_SOURCE
            screen_env_bindings.cpp
            envs/BaseEnvironment.hpp
            envs/ScreenEnvironment.hpp)

    pybind11_add_module(agario_screen_env ${AGARIO_SCREEN_ENV_SOURCE})
    target_include_directories(agario_screen_env PRIVATE "../agario" ${OPENGL_INCLUDE_DIR})
    target_link_libraries(agario_screen_env PUBLIC ${OPENGL_LIBRARIES} glm glfw)
else()
    message("OpenGL not found")
endif()

# the following works outside of cmake
# g++ -O3 -Wall -shared -std=c++11 -fPIC `python -m pybind11 --includes`
# bindings.cpp -o env`python3-config --extension-suffix` -undefined dynamic_lookup
