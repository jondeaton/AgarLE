#pragma once

#include "core/Ball.hpp"
#include "core/Entities.hpp"
#include "core/Player.hpp"

namespace agario {

  template<bool renderable>
  class GameState {
  public:
    std::vector<agario::Player<renderable>> players;
    std::vector<agario::Pellet<renderable>> pellets;
    std::vector<agario::Food<renderable>> foods;
    std::vector<agario::Virus<renderable>> viruses;

    // todo: serialization and deserialization
  };

}