#ifndef PROJECT_AGARIO_HPP
#define PROJECT_AGARIO_HPP

#include <vector>

#include "types.hpp"
#include "Player.hpp"
#include "Entities.hpp"

#include "THST/QuadTree.h"

namespace Agario {

  class Engine {
  public:
    explicit Engine() : ticks(0) {}

    void tick();

    int num_players() { return players.size(); }
    int num_food() { return foods.size(); }
    int num_viruses() { return viruses.size(); }
    int num_masses() { return masses.size(); }

  private:
    std::vector<Player> players;

    std::vector<Food> foods;
    std::vector<Mass> masses;
    std::vector<Virus> viruses;

//  spatial::QuadTree<position, Box2<position>, 2> qtree();

    void add_food(int num_food);
    void add_virus(int num_virus);

    void tick_player(Player &player);
    void move_player(Player &player);
    Agario::tick ticks;
  };



}


#endif //PROJECT_AGARIO_HPP
