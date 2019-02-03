
#include "Engine.hpp"
#include <iostream>

namespace Agario {
  
  void Engine::tick() {
    std::cout << "tick!" << std::endl;

    for (Player &player : players)
      tick_player(player);

    ticks++;
  }

  void Engine::tick_player(Player &player) {
    move_player(player);

    for (Cell &cell : player.cells) {

      for (Food &food : foods) {
        if (cell.collides_with(food)) {

        }
      }

      for (Virus &virus : viruses) {
        if (cell.collides_with(virus)) {

        }
      }

      for (Mass &mass : masses) {

      }


    }

    for (Player &other : players) {

    }

  }

  void Engine::move_player(Player &player) {
    // todo
  }


  void Engine::add_food(int num_food) {
    //todo
  }

  void Engine::add_virus(int num_virus) {
    // todo
  }

}

