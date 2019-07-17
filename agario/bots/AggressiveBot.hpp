#pragma once

#include "core/Player.hpp"
#include <bots/Bot.hpp>

#define AGGRESSIVE_RADIUS 25

namespace agario {
  namespace bot {

    template<bool renderable>
    class AggressiveBot : public Bot<renderable> {
      /* this is an example Bot. Use it as a template to write other ones. */
      typedef Bot<renderable> Bot;
      typedef agario::Player<renderable> Player;
      static constexpr agario::color default_color = agario::color::red;

    public:
      AggressiveBot(agario::pid pid, const std::string &name, agario::color color) : Bot(pid, name, color), splitting(false) {}
      AggressiveBot(agario::pid pid, const std::string &name) : AggressiveBot(pid, name, default_color) {}
      explicit AggressiveBot(const std::string &name) : AggressiveBot(-1, name) {}
      explicit AggressiveBot(agario::pid pid) : AggressiveBot(pid, "AggressiveBot") {}

      void take_action(const GameState<renderable> &state) override {
        this->action = agario::action::none; // no splitting or anything
        this->target = this->nearest_pellet(state);
      }

    private:
      bool splitting;

    };
  }
}