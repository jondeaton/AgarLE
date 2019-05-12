# Agar.io Learning Environment (AgarLE)

This project presents a performant re-implementation of the popular online multi-player
game agar.io and a Python interface suitable for reinforcement learning.

<img width="200" alt="screenshot" src="https://user-images.githubusercontent.com/15920014/57587859-dbb31400-74c0-11e9-8f47-3e39113b99b4.png">

# Set up and installation

First, build the agario C++ environment

    mkdir build && cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make agario_full_env

This outputs a python-importable `*.so` file called `agario_full_env`. To
use it, copy it into the "site packages" for your Python interpreter like so:

    cp environment/agario_* ~/.local/lib/python3.7/site-packages/

Next, install the OpenAI gym environment wrapper

    python setup.py install

The `agario_full_env` environment can be compiled such that calling `render()`
will render the game onto the screen. This feature is turned off by default
but can be turned on during compilation by using the following cmake
command instead of the one shown above. 

    cmake -DCMAKE_BUILD_TYPE=Release -DDEFINE_RENDERABLE=ON ..
