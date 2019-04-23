# AgarAI

Reinforcement Learning environment for Agar.io

# todo
- Basic game mechanics
  - Feeding
  - time-dependent deceleration on splitting
  - Player starts in random location
  - Virus mass aligns with it's size
  - Arena mass conservation
  - Fix bug when many small cells are on the edge
  - Fix exception on window close
  - Fix view on death (add player death state)
  - Make split/feed cooldown time based
- AgarLE (Learning Environment)
  - Make new CMake Project in other directory
  - Design state representation(s)
- Client-Server connection
  - GameState serialization
  - Write server I/O
  - Write client I/O

- Refactoring
  - Benchmarks for game ticking
  - More efficient algorithms for entity collisions
  - Better solution to NumberWrapper (is there one?)
