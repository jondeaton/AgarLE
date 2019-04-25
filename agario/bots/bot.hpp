#pragma once

#include <core/Player.hpp>
#include <engine/GameState.hpp>

namespace agario::bot {

  template<bool renderable>
  class Bot : public agario::Player<renderable> {
  public:
    typedef Player <renderable> Player;
    using Player::Player;

    explicit Bot(std::string name) : Player(name) {}

    virtual void take_action(const GameState <renderable> &state) = 0;

    virtual ~Bot() = default;

    Bot(const Bot & /* other */) = default;
    Bot &operator=(const Bot & /* other */) = default;
    Bot(Bot && /* other */) noexcept = default;
    Bot &operator=(Bot && /* other */) noexcept = default;
  };

}