#pragma once

#include "core/Ball.hpp"
#include "core/Entities.hpp"
#include "core/Player.hpp"

#include <vector>
#include <unordered_map>

namespace agario {

  template<bool renderable>
  class GameState {
  public:
    std::unordered_map<agario::pid, std::shared_ptr<agario::Player<renderable>>> players;
    std::vector<agario::Pellet<renderable>> pellets;
    std::vector<agario::Food<renderable>> foods;
    std::vector<agario::Virus<renderable>> viruses;

    void clear() {
      players.clear();
      pellets.clear();
      foods.clear();
      viruses.clear();
    }
  };

  template<bool r>
  std::ostream &operator<<(std::ostream &os, const GameState<r> &state) {
    std::vector<std::shared_ptr<agario::Player<r>>> leaderboard;

    using pp = std::shared_ptr<Player<r>>;

    for (auto &pair : state.players) {
      auto
        it = std::lower_bound(leaderboard.begin(), leaderboard.end(), pair.second,
                              [&](const pp &p1, const pp &p2) {
                                return *p1 > *p2;
                              });
      leaderboard.insert(it, pair.second);
    }

    for (unsigned i = 0; i < leaderboard.size(); ++i)
      os << i + 1 << ".\t" << *leaderboard[i] << std::endl;

    return os;
  }

}