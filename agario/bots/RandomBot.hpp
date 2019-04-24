#pragma once

#include "bots/bot.hpp"
#include "engine/Engine.hpp"

namespace agario::bot {

  template<bool renderable>
  class RandomBot : public agario::bot::Bot<renderable> {
  public:
    typedef agario::bot::Bot<renderable> Bot;
    using Bot::Bot; // inherit constructors

    RandomBot(std::string name) : Bot(name) { }

    void take_action(const GameState<renderable> &state) {
      static_cast<void>(state); // unused

      this->action = agario::action::none;

      this->target.x = this->x() + (std::rand() % 10 - 5);
      this->target.y = this->y() + (std::rand() % 10 - 5);
    }

  };

}