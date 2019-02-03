#ifndef PROJECT_AGARIO_HPP
#define PROJECT_AGARIO_HPP

#include <vector>
#include <cstdlib>

#include "settings.hpp"
#include "types.hpp"
#include "Player.hpp"
#include "Entities.hpp"

#include "THST/QuadTree.h"

namespace Agario {

  class Engine {
  public:
    explicit Engine(position canvas_width, position canvas_height) :
    ticks(0), canvas_width(canvas_width), canvas_height(canvas_height) {
      std::srand(SEED);
    }

    std::vector<Player> &leaderboard();

    void tick();

    int total_players() { return players.size(); }
    int total_pellets() { return pellets.size(); }
    int total_viruses() { return viruses.size(); }
    int total_foods() { return foods.size(); }


  private:
    std::vector<Player> players;

    std::vector<Pellet> pellets;
    std::vector<Food> foods;
    std::vector<Virus> viruses;

    position canvas_width;
    position canvas_height;

    Agario::tick ticks;

//  spatial::QuadTree<position, Box2<position>, 2> qtree();

    void add_pellets(int num_pellets);
    void add_virus(int num_virus);

    void tick_player(Player &player);
    void move_player(Player &player);
    void eat_pellets(Cell &cell);
    void eat_food(Cell &cell);
    void check_virus_collisions(Cell &cell, std::vector<Cell> &created_cells);
    void disrupt(Cell &cell, std::vector<Cell> &created_cells);

    Agario::Location random_location();
  };
}


#endif //PROJECT_AGARIO_HPP
