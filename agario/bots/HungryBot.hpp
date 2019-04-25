#pragma once

#include "core/Player.hpp"

namespace agario::bot {

    template<bool renderable>
    class HungryBot : public agario::Player<renderable> {
    public:
      typedef agario::Player<renderable> Player;

      template<typename Loc>
      HungryBot(agario::pid pid, std::string name, Loc &&loc, agario::color color) :
      Player(pid, name, loc, color) { }

      HungryBot(agario::pid pid, std::string name, agario::color color) :
        HungryBot(pid, name, Location(0, 0), color) {}

      HungryBot(agario::pid pid, std::string name) : HungryBot(pid, name, agario::color::blue) {}
      HungryBot(std::string name) : HungryBot(-1, name, agario::color::blue) {}

      void take_action(const GameState<renderable> &state) override {
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