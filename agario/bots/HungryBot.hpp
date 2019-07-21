#pragma once

#include <agario/core/Player.hpp>
#include <agario/bots/Bot.hpp>

namespace agario {
  namespace bot {

    template<bool renderable>
    class HungryBot : public Bot<renderable> {
    public:
      typedef Bot<renderable> Bot;

      HungryBot(agario::pid pid, const std::string &name, agario::color color) : Bot(pid, name, color) {}
      HungryBot(agario::pid pid, const std::string &name) : HungryBot(pid, name, agario::color::blue) {}
      explicit HungryBot(const std::string &name) : HungryBot(-1, name) {}
      explicit HungryBot(agario::pid pid) : HungryBot(pid, "HungryBot") {}

      void take_action(const GameState <renderable> &state) override {
        this->action = agario::action::none;
        this->target = this->nearest_pellet(state);
      }

    };


  }
}