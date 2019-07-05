#pragma once

#include "core/Player.hpp"

namespace agario {
  namespace bot {

    template<bool renderable>
    class RandomBot : public agario::Player<renderable> {
    public:
      typedef agario::Player<renderable> Player;

      RandomBot(agario::pid pid, const std::string &name, agario::color color) : Player(pid, name, color) {}
      RandomBot(agario::pid pid, const std::string &name) : RandomBot(pid, name, agario::color::red) {}
      RandomBot(const std::string &name) : RandomBot(-1, name, agario::color::blue) {}

      void take_action(const GameState <renderable> &state) override {
        static_cast<void>(state); // unused

        this->action = agario::action::none;

        this->target.x = this->x() + (std::rand() % 10 - 5);
        this->target.y = this->y() + (std::rand() % 10 - 5);
      }

    };


  }
}