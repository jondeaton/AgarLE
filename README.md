# AgarLE (Agar.io Learning Environment)

This project presents a performant re-implementation of the popular online multi-player
game agar.io and a Python interface suitable for reinforcement learning.


# todo
- Learning Environment
  - image isn't centered on player
  - make focused player some special color
  - get rid of dock icon (how?)
  - environment messes up matplotlib
  - configurable width and height
- Bugs
  - splitting into bottom left corner breaks the game
    - fix this by clipping position *after movement*
- Game Mechanics Tweaking
  - Feeding viruses causing them to duplicate
  - big cells should move/split faster (same split speed?)
  - arena mass conservation?
- Refactoring
  - Get rid of debugging targets
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

