# AgarAI

Reinforcement Learning environment for Agar.io

# todo
- Get basic game mechanics
  - Handle player keyboard inputs (split/feed)
  - Entity interactions (several of these)
    - Player's cells don't overlap
- Refactoring
  - restructure class structure to avoid Diamond problem (worth it?)
  - better solution to NumberWrapper (is there one?)
- Client-Server connection
  - GameState serialization
  - Write server I/O
  - Write client I/O
- Bind engine to AgarLE with PyBind
  - Write C++ environment
  - Get images without rendering them
  - Write basic opponent algorithms
  - Write non-visual (i.e. RAM) state space

