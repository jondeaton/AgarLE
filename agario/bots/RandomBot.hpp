#pragma once

#include "core/Player.hpp"

namespace agario::bot {

  template<bool renderable>
  class RandomBot : public agario::Player<renderable> {
  public:
    typedef agario::Player<renderable> Player;

    template<typename Loc>
    RandomBot(agario::pid pid, std::string name, Loc &&loc, agario::color color) : Player(pid, name, loc, color) { }
    template<typename Loc>
    RandomBot(agario::pid pid, std::string name, Loc &&loc) : RandomBot(pid, name, loc, agario::color::red) {}
    RandomBot(agario::pid pid, std::string name, agario::color color) : RandomBot(pid, name, Location(0, 0), color) {}
    RandomBot(agario::pid pid, std::string name) : RandomBot(pid, name, agario::color::red) {}
    RandomBot(std::string name) : RandomBot(-1, name, agario::color::blue) {}

    void take_action(const GameState<renderable> &state) override {
      static_cast<void>(state); // unused

      this->action = agario::action::none;

      this->target.x = this->x() + (std::rand() % 10 - 5);
      this->target.y = this->y() + (std::rand() % 10 - 5);
    }

  };

}