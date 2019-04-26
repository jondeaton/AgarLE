# AgarAI

Reinforcement Learning environment for Agar.io

# todo
- AgarLE (Learning Environment)
  - Make new CMake Project in other directory
  - Render game without open window
  - Design state representation(s)
  - Wrap in OpenAI Gym Environment
- Bugs
  - splitting into bottom left corner breaks the game
  - player doesn't have name?
- Game Mechanics Tweaking
  - Feeding viruses causing them to duplicate
  - big cells should move and split faster
  - slow taking away of mass from big cells?
  - split/feed cooldown time based (not tick based)
- Refactoring
  - Benchmarks for game ticking
  - More efficient algorithms for entity collisions
  - Better solution to NumberWrapper (is there one?)
  - make speed and time new types?

- Client-Server connection
  - GameState serialization
  - Server IO
  - Client IO

