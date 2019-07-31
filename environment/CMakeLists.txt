cmake_minimum_required(VERSION 3.5)
project (AvarioEnv)

set(CMAKE_CXX_STANDARD 14)

include_directories("." "..")
add_subdirectory(pybind11)

IF(APPLE)
    # Fix linking on 10.14+. See https://stackoverflow.com/questions/54068035
    link_directories(/usr/local/lib)
ENDIF()

option(DEFINE_RENDERABLE "Renderable" OFF) # Disabled by default
if(DEFINE_RENDERABLE)
    message("Renderable")
    add_definitions(-DRENDERABLE)
endif(DEFINE_RENDERABLE)


set(AGARIO_ENVS_SOURCE
        envs/BaseEnvironment.hpp
        envs/FullEnvironment.hpp
        envs/GridEnvironment.hpp
        envs/RamEnvironment.hpp)

set(AGARIO_SCREEN_ENV_SOURCE
        envs/BaseEnvironment.hpp
        envs/ScreenEnvironment.hpp)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
if (OpenGL_FOUND)

    add_definitions(-DINCLUDE_SCREEN_ENV)

    find_library(GLFW REQUIRED)
    find_package(glfw3)
    find_package(glm REQUIRED)

    include_directories(${GLM_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR} ${GLFW_INCLUDE_DIR})
    link_directories(${GLM_LIBRARY_DIRS})
    add_definitions(${GLM_DEFINITIONS})

    pybind11_add_module(agarle bindings.cpp
            ${AGARIO_ENVS_SOURCE}
            ${AGARIO_SCREEN_ENV_SOURCE})

    target_include_directories(agarle PRIVATE "..")
    target_include_directories(agarle  PRIVATE "../agario" ${OPENGL_INCLUDE_DIR} ${GLM_INCLUDE_DIRS})
    target_link_libraries(agarle PUBLIC ${OPENGL_LIBRARIES} glm glfw)

else()
    message("OpenGL not found")

    pybind11_add_module(agarle bindings.cpp
            ${AGARIO_ENVS_SOURCE})
    target_include_directories(agarle PRIVATE "..")

endif()



# ============================================================
# Testing
# ============================================================

find_package(gtest)
if (GTEST_FOUND)
    # set(TEST_FLAGS "-g -O0 -Wall -Wextra -pedantic")

    set(TEST_SRC
            test/main.cpp
            test/grid-test.hpp)
    include_directories(test ${GTEST_INDLUCE_DIRS})

    add_executable(test-envs ${TEST_SRC} ${AGARIO_GRID_ENV_SOURCE})
    target_include_directories(test-envs PRIVATE "..")
    target_link_libraries(test-envs gtest pthread)
    #    target_compile_options(test-engine PUBLIC ${TEST_FLAGS})
else()
    message("Google Test not found")
endif()

# the following works outside of cmake
# g++ -O3 -Wall -shared -std=c++11 -fPIC `python -m pybind11 --includes`
# bindings.cpp -o env`python3-config --extension-suffix` -undefined dynamic_lookup
