#pragma once

#include <agario/core/Player.hpp>
#include <agario/bots/Bot.hpp>

namespace agario {
  namespace bot {

    template<bool renderable>
    class ExampleBot : public Bot<renderable> {
      /* this is an example Bot. Use it as a template to write other ones. */
      typedef agario::Player<renderable> Player;
      typedef Bot<renderable> Bot;
      static constexpr agario::color default_color = agario::color::yellow;

    public:
      // constructors that mirror those declared in Player (try not to touch these... it'll make you very sad)
      ExampleBot(agario::pid pid, const std::string &name, agario::color color) : Bot(pid, name, color) {}
      ExampleBot(agario::pid pid, const std::string &name) : ExampleBot(pid, name, default_color) {}
      explicit ExampleBot(const std::string &name) : ExampleBot(-1, name) {}
      explicit ExampleBot(agario::pid pid) : ExampleBot(pid, "ExampleBot") {}

      /**
       * Example take_action function. This function is called on the bot
       * during every game tick, allowing the bot to act differently
       * on each tick of the game. In MDP terms, this function is the
       * agent's policy. To use this function, set the "action" and "target"
       * fields of the bot
       *
       *  - action can be either "split", "feed", or "none".
       *
       *  - target is the location in world space where the agent should move towards.
       *    When you're actually playing the game, this is equal to the location
       *    of your mouse's cursor in the game.
       *
       * @param state a structure containing the current game state, including
       * all of the locations of every player's cells, foods, pellets, and viruses
       * at this moment. Smart bots use this information to make informed actions
       * such as "go towards the nearest food" or "run away from a big player
       * if they are nearby". This example bot just does nothing and stays where it is.
       */
      void take_action(const GameState<renderable> &state) override {
        static_cast<void>(state); // unused

        // example: do nothing, just stay where you are
        this->action = agario::action::none;  // don't split, don't feed (i.e. do nothing)
        this->target = this->location(); // go towards where I already am
      }

    };
  }
}