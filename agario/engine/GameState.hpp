#pragma once

#include "core/Ball.hpp"
#include "core/Entities.hpp"
#include "core/Player.hpp"

#include <vector>
#include <unordered_map>
#include <iomanip>

namespace agario {

  template<bool renderable>
  class GameState {
  public:
    std::unordered_map<agario::pid, std::shared_ptr<agario::Player<renderable>>> players;
    std::vector<agario::Pellet<renderable>> pellets;
    std::vector<agario::Food<renderable>> foods;
    std::vector<agario::Virus<renderable>> viruses;

    agario::distance arena_width;
    agario::distance arena_height;

    explicit GameState (agario::distance arena_width, agario::distance arena_height) :
      arena_width(arena_width), arena_height(arena_height) { }

    void clear() {
      players.clear();
      pellets.clear();
      foods.clear();
      viruses.clear();
    }
  };

  /* prints out a list of players sorted by mass (i.e. the leaderboard) */
  template<bool r>
  std::ostream &operator<<(std::ostream &os, const GameState<r> &state) {

    // make a sorted list of (pointers to) players
    std::vector<std::shared_ptr<agario::Player<r>>> leaderboard;
    using pp = std::shared_ptr<Player<r>>;
    for (auto &pair : state.players) {
      auto it = std::lower_bound(leaderboard.begin(), leaderboard.end(), pair.second,
                              [&](const pp &p1, const pp &p2) {
                                return *p1 > *p2;
                              });
      leaderboard.insert(it, pair.second);
    }

    // print them out in sorted order
    for (unsigned i = 0; i < leaderboard.size(); ++i) {
      os << i + 1 << ".\t" << std::setfill(' ') << std::setw(5);
      auto &player = *leaderboard[i];
      os <<  player.mass() << "\t" << player << std::endl;
    }
    return os;
  }

}