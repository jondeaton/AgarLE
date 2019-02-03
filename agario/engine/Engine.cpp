
#include "Engine.hpp"
#include <iostream>

void AgarioEngine::tick() {
  std::cout << "tick!" << std::endl;

  for (Player &player : players)
    tick_player(player);

  ticks++;
}

void AgarioEngine::tick_player(Player &player) {

  move_player(player);

  for (Player &other : players) {

  }

}

void AgarioEngine::move_player(Player &player) {
  // todo
}


void AgarioEngine::add_food(int num_food) {
  //todo
}

void AgarioEngine::add_virus(int num_virus) {
  // todo
}
