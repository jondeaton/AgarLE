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
    std::unordered_map<agario::pid, agario::Player<renderable>> players;
    std::vector<agario::Pellet<renderable>> pellets;
    std::vector<agario::Food<renderable>> foods;
    std::vector<agario::Virus<renderable>> viruses;

    // todo: serialization and deserialization
  };

}