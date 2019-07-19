#pragma once

#include <agario/engine/GameState.hpp>
#include <agario/core/Player.hpp>

namespace agario {
  namespace bot {

    template<bool renderable>
    class Bot : public agario::Player<renderable> {
      /* this is an example Bot. Use it as a template to write other ones. */
      typedef agario::Player<renderable> Player;
      static constexpr agario::color default_color = agario::color::yellow;

    public:
      Bot(agario::pid pid, const std::string &name, agario::color color) : Player(pid, name, color) {}
      Bot(agario::pid pid, const std::string &name) : Bot(pid, name, default_color) {}
      explicit Bot(const std::string &name) : Bot(-1, name) {}
      explicit Bot(agario::pid pid) : Bot(pid, "Bot") {}

    protected:

      /* location of the nearest pellet */
      agario::Location nearest_pellet (const GameState<renderable> &state) {
        distance min_distance = agario::distance::max();
        agario::Location target;
        for (auto &pellet : state.pellets) {
          distance dist = pellet.location().distance_to(this->location());
          if (dist < min_distance) {
            target = pellet.location();
            min_distance = dist;
          }
        }
        return target;
      }

      /* location of the nearest food */
      agario::Location nearest_food (const GameState<renderable> &state) {
        distance min_distance = agario::distance::max();
        agario::Location target;
        for (auto &food : state.foods) {
          distance dist = food.location().distance_to(this->location());
          if (dist < min_distance) {
            target = food.location();
            min_distance = dist;
          }
        }
        return target;
      }

    };
  }
}