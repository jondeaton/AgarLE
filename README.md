<img width="380" alt="Screen Shot 2019-07-01 at 5 13 18 PM" src="https://user-images.githubusercontent.com/15920014/60447827-9d1a1a00-9c24-11e9-8a8b-a8043e8e1302.png">

The Agar.io Learning Environment (AgarLE) is a performant implementation of the popular online multi-player game agar.io along with an [OpenAI Gym](https://gym.openai.com/) interface suitable for reinforcement learning in Python.

<p align="center">
<img width="460" alt="screenshot" src="https://user-images.githubusercontent.com/15920014/57587859-dbb31400-74c0-11e9-8f47-3e39113b99b4.png">
</p>

# Installation
You can build and install the agario gym interace simply by running

    python setup.py install

# Advanced setup
To build the client (to play the game yourself) or to build a version 
of the gym environment that can be rendered, use the following advanced
setup guide

    mkdir build && cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make -j 2 client agarle

This will output an executable named `client` which you can run  like so

    agario/client

This will also output a python-importable `*.so` file called `agarle`. To
use it, copy it into the "site packages" for your Python interpreter like so:

    cp environment/agarle* ~/.local/lib/python3.7/site-packages/

The `agario_full_env` environment can be compiled such that calling `render()`
will render the game onto the screen. This feature is turned off by default
but can be turned on during compilation by using the following cmake
command instead of the one shown above. 

    cmake -DCMAKE_BUILD_TYPE=Release -DDEFINE_RENDERABLE=ON ..
