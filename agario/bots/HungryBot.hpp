#pragma once

#include "core/Player.hpp"

namespace agario {
  namespace bot {

    template<bool renderable>
    class HungryBot : public agario::Player<renderable> {
    public:
      typedef agario::Player<renderable> Player;

      HungryBot(agario::pid pid, const std::string &name, agario::color color) : Player(pid, name, color) {}
      HungryBot(agario::pid pid, const std::string &name) : HungryBot(pid, name, agario::color::blue) {}
      HungryBot(const std::string &name) : HungryBot(-1, name) {}
      HungryBot(agario::pid pid) : HungryBot(pid, "HungryBot") {}

      void take_action(const GameState <renderable> &state) override {
        this->action = agario::action::none;

        distance min_distance = agario::distance::max();

        for (auto &pellet : state.pellets) {
          distance dist = pellet.location().distance_to(this->location());
          if (dist < min_distance) {
            this->target = pellet.location();
            min_distance = dist;
          }
        }
      }

    };


  }
}