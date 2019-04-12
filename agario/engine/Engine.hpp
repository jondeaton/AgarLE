#pragma once

#include <vector>
#include <cstdlib>

#include "core/Player.hpp"
#include "settings.hpp"
#include "core/types.hpp"
#include "core/Entities.hpp"

namespace agario {

  class Engine {
  public:

    Engine() : arena_width(DEFAULT_ARENA_WIDTH), arena_height(DEFAULT_ARENA_HEIGHT),
               ticks(0) {
      std::srand(SEED);
    }

    explicit Engine(distance arena_width, distance arena_height) :
    arena_width(arena_width), arena_height(arena_height), ticks(0) {
      std::srand(SEED);
    }

    // set all entities to their un-renderable versions
    typedef Player<false> Player;
    typedef Cell<false> Cell;
    typedef Food<false> Food;
    typedef Pellet<false> Pellet;
    typedef Virus<false> Virus;

    std::vector<Player> &leaderboard();

    void tick();

    int total_players() { return players.size(); }
    int total_pellets() { return pellets.size(); }
    int total_viruses() { return viruses.size(); }
    int total_foods() { return foods.size(); }

    Engine(const Engine&) = delete; // no copy constructor
    Engine& operator=(const Engine&) = delete; // no copy assignments
    Engine(Engine&&) = delete; // no move constructor
    Engine& operator=(Engine&&) = delete; // no move assignment

  private:
    std::vector<Player> players;

    std::vector<Pellet> pellets;
    std::vector<Food> foods;
    std::vector<Virus> viruses;

    distance arena_width;
    distance arena_height;

    agario::tick ticks;

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

    agario::Location random_location();
  };
}

