#pragma once

#include <bots/bot.hpp>

#define SHY_RADIUS 25

namespace agario::bot {

  template<bool renderable>
  class HungryShyBot : public agario::Player<renderable> {
  public:

    typedef agario::Player<renderable> Player;

    template<typename Loc>
    HungryShyBot(agario::pid pid, std::string name, Loc &&loc, agario::color color) :
    Player(pid, name, loc, color) { }

    HungryShyBot(agario::pid pid, std::string name, agario::color color) :
      HungryShyBot(pid, name, Location(0, 0), color) {}

    HungryShyBot(agario::pid pid, std::string name) : HungryShyBot(pid, name, agario::color::blue) {}
    HungryShyBot(std::string name) : HungryShyBot(-1, name, agario::color::blue) {}


    void take_action(const GameState<renderable> &state) override {
      this->action = agario::action::none; // no splitting or anything

      // check if there are any big players nearby
      for (auto &pair : state.players) {
        Player &other_player = *pair.second;

        // is it nearby?
        auto distance = this->location().distance_to(other_player.location());
        if (distance > SHY_RADIUS)
          continue;

        // it is scary?
        if (other_player.mass() > mass()) {
          // yes! run (directly) away!
          this->target = this->location() - (other_player.location() - this->location());
          return;
        }
      }

      // no cells are too close for comfort... forage for foods
      distance min_distance = std::numeric_limits<distance>::infinity();

      for (auto &pellet : state.pellets) {
        distance dist = pellet.location().distance_to(this->location());
        if (dist < min_distance) {
          this->target = pellet.location(); // i can haz cheeseburger?
          min_distance = dist;
        }
      }
    }

  };

}