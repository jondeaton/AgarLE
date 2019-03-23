#pragma once

#include <vector>
#include <cstdlib>

#include "Player.hpp"
#include "settings.hpp"
#include "types.hpp"
#include "Entities.hpp"

namespace Agario {

  class Engine {
  public:
    explicit Engine(distance canvas_width, distance canvas_height) :
    canvas_width(canvas_width), canvas_height(canvas_height), ticks(0) {
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

    distance canvas_width;
    distance canvas_height;

    Agario::tick ticks;


    void add_pellets(int num_pellets);
    void add_virus(int num_virus);

    void tick_player(Player &player);
    void move_player(Player &player);

    void eat_pellets(Cell &cell);
    void eat_food(Cell &cell);

    // actions
    void emit_foods(Player &player);
    void player_split(Player &player, std::vector<Cell>& created_cells);
    void check_player_collisions(Player &player);
    void eat_others(Player &player, Cell &cell);

    void recombine_cells(Player &player);
    void check_virus_collisions(Cell &cell, std::vector<Cell> &created_cells);
    void disrupt(Cell &cell, std::vector<Cell> &created_cells);

    Agario::Location random_location();

    Engine(const Engine&) = delete; // no copy constructor
    Engine& operator=(const Engine&) = delete; // no copy assignments
    Engine(Engine&&) = delete; // no move constructor
    Engine& operator=(Engine&&) = delete; // no move assignment
  };
}

