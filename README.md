<img width="380" alt="Screen Shot 2019-07-01 at 5 13 18 PM" src="https://user-images.githubusercontent.com/15920014/60447827-9d1a1a00-9c24-11e9-8a8b-a8043e8e1302.png">

The Agar.io Learning Environment (AgarLE) is a performant implementation of the popular online multi-player game agar.io along with an [OpenAI Gym](https://gym.openai.com/) interface suitable for reinforcement learning in Python.

<p align="center">
<img width="460" alt="screenshot" src="https://user-images.githubusercontent.com/15920014/57587859-dbb31400-74c0-11e9-8f47-3e39113b99b4.png">
</p>

# Installation
To install: clone this repository and initialize it's submodules

    git clone --recursive https://github.com/jondeaton/AgarLE.git

and then just run the installation script

    python setup.py install

# Example

```python
import gym
import gym_agario
    
env = gym.make("agario-grid-v0")
    
game_state = env.reset()
game_state.shape # (128, 128, 10) , (grid_size, grid_size, num_channels)

action = np.array([0, 0]), 0  # don't move, don't split
while True:
  game_state, reward, done, info = env.step(action)
  if done: break
```

You can also configure the Agario game and observations space like so:

```python
config = {
  'ticks_per_step':  4,     # Number of game ticks per step
  'num_frames':      1,     # Number of ticks of gameplay observed after each step
  'arena_size':      1000,  # Size of game arena
  'num_pellets':     1000,
  'num_viruses':     25,
  'num_bots':        25,
  'pellet_regen':    True,  # Whether pellets regenerate when eaten
  'grid_size':       128,   # Size of spatial dimentions of observations
  'observe_cells':   True,  # Include an observation channel with player cells
  'observe_others':  True,  # Include an observation channel with other players' cells
  'observe_viruses': True,  # Include an observation channel with viruses
  'observe_pellets': True   # Include an observation channel with pellets
}

env = gym.make("agario-grid-v0", **config)
```

# Caveats

Currently compilation/installation is only working with Clang, so if you're
on Linux then you'll need to set your C++ compiler to Clang in your environment
before installing.

    CXX=`which clang++`

The only enviornment which has been tested extensively is `agario-grid-v0`,
although the RAM environment `agario-ram-v0` and screen environment `agario-screen-v0`
should work with some coaxing. The `agario-screen-v0` requires a window manager to
work so will not work on headless Linux machines, for instance. Calling `render`
will only work if the executable has been built with rendering turned on as can be
done by following the advanced set up guide. Rendering will not work
with the "screen" environment, despite the fact that that environment uses
the screen image as the environment's observation.

# Advanced setup
In order to play the game yourself or enable rendering in the gym environment,
you will need to build the game client yourself on a system where OpenGL has
been installed. To do so, issue the following commands

    git submodule update --init --recursive
    mkdir build && cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make -j 2 client agarle

This will output an executable named `client` in the directory `agario`

    agario/client  # play the game

If you also build the `agarle` target, then a python-importable dynamic library
(i.e. `*.so` file) named `agarle` will have been produced. To use it, copy it
into the "site packages" for your Python interpreter like so:

    cp environment/agarle* `python -m site --user-site`

The underlying gym environments may be compiled such that calling `render()`
will render the game onto the screen. This feature is turned off by default
for performance and portability reasons, but can be turned on during
compilation by using the following cmake command instead of the one shown above. 

    cmake -DCMAKE_BUILD_TYPE=Release -DDEFINE_RENDERABLE=ON ..
