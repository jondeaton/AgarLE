#pragma once

#include <bots/bot.hpp>

namespace agario::bot {

    template<bool renderable>
    class HungryBot : public agario::bot::Bot<renderable> {
    public:
      typedef agario::bot::Bot<renderable> Bot;
      using Bot::Bot; // inherit constructors

      HungryBot(std::string name) : Bot(name) { }

      void take_action(const GameState<renderable> &state) {
        this->action = agario::action::none;

        distance min_distance = std::numeric_limits<distance>::infinity();

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