<img width="380" alt="Screen Shot 2019-07-01 at 5 13 18 PM" src="https://user-images.githubusercontent.com/15920014/60447827-9d1a1a00-9c24-11e9-8a8b-a8043e8e1302.png">

The Agar.io Learning Environment (AgarLE) is a performant implementation of the popular online multi-player
game agar.io along with an [OpenAI Gym](https://gym.openai.com/) interface suitable for reinforcement learning in Python.

<p align="center">
<img width="460" alt="screenshot" src="https://user-images.githubusercontent.com/15920014/57587859-dbb31400-74c0-11e9-8f47-3e39113b99b4.png">
</p>

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
