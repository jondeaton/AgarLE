cmake_minimum_required(VERSION 3.5)
project (agario-environment)

add_subdirectory(pybind11)
pybind11_add_module(agario_env bindings.cpp)

# g++ -O3 -Wall -shared -std=c++11 -fPIC `python -m pybind11 --includes` bindings.cpp -o env`python3-config --extension-suffix` -undefined dynamic_lookup
