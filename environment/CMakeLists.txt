cmake_minimum_required(VERSION 3.5)
project (AvarioEnv)

include_directories(".")
add_subdirectory(pybind11)

set(AGARIO_ENV_SOURCE
        bindings.cpp
        environment.hpp)

pybind11_add_module(agario_env ${AGARIO_ENV_SOURCE})

target_include_directories(agario_env PRIVATE "../agario")
target_link_libraries(agario_env PRIVATE agario)

# the following works outside of cmake
# g++ -O3 -Wall -shared -std=c++11 -fPIC `python -m pybind11 --includes`
# bindings.cpp -o env`python3-config --extension-suffix` -undefined dynamic_lookup
