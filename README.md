# AgarLE (Agar.io Learning Environment)

This project presents a performant re-implementation of the popular online multi-player
game agar.io and a Python interface suitable for reinforcement learning.


# Set up

Build the agario C++ environment

    mkdir build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make agario_full_env

Install it in global location for python import

    cp environment/agario_* ~/.local/lib/python3.7/site-packages/

Install gym_agario

    python setup.py install

# todo
- Learning Environment
  - image isn't centered on player
  - make focused player some special color
  - get rid of dock icon (how?)
  - configurable width and height of screen
- Game Mechanics Tweaking
  - small cells can push big cells
  - Feeding viruses causing them to duplicate
  - arena mass conservation?
 - Bugs
    - viruses appear inside players = splitting them
    - splitting into bottom left corner breaks the game
      - fix this by clipping position *after movement*
- Refactoring
  - document everything a lot
  - Benchmarks for game ticking
  - More efficient algorithms for entity collisions
  - Better solution to NumberWrapper (is there one?)
  - make speed and time new types?
- Client-Server connection
  - GameState serialization
  - Server IO
  - Client IO 
  - browser client (WASM + WebGL?)

