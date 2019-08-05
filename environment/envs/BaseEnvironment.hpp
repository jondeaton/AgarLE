#pragma once

#include <agario/engine/Engine.hpp>
#include <agario/core/types.hpp>
#include <agario/core/Entities.hpp>
#include <agario/core/Ball.hpp>
#include <agario/bots/bots.hpp>
#include "agario/engine/GameState.hpp"

// 60 frames per second: the default amount of time between frames of the game
#define DEFAULT_DT (1.0 / 60)

namespace agario {
  namespace env {

    class EnvironmentException : public std::runtime_error {
      using runtime_error::runtime_error;
    };

      typedef double reward;

      template<bool renderable>
      class BaseEnvironment {
        using Player = agario::Player<renderable>;
        using HungryBot = agario::bot::HungryBot<renderable>;
        using HungryShyBot = agario::bot::HungryShyBot<renderable>;

      public:

        explicit BaseEnvironment(int ticks_per_step, int arena_size, bool pellet_regen,
                                 int num_pellets, int num_viruses, int num_bots) :
          engine(arena_size, arena_size, num_pellets, num_viruses, pellet_regen),
          _ticks_per_step(ticks_per_step), num_bots(num_bots), _done(false),
          step_dt(DEFAULT_DT) {
          pid = engine.template add_player<Player>("agent");
          reset();
        }

        /**
         * Steps the environment forward by several game frames
         * @return the reward accumulated by the player during those
         * frames, which is equal to the difference in it's mass before
         * and after the step
         */
        reward step() {
          auto &player = engine.player(pid);
          auto mass_before = static_cast<int>(player.mass());
          for (int i = 0; i < ticks_per_step(); i++) {
            if (player.dead()) {
              _done = true;
              break;
            }
            engine.tick(step_dt);
            this->_partial_observation(player, i);
          }

          auto mass_now = static_cast<int>(player.mass());
          auto reward = mass_now - mass_before;
          return reward;
        }

        /**
         * Specifies the next action for the agent to take
         * but does not step the game forwards in time. This
         * just specifies what action will be taken by
         * the agent on the next call to step
         * @param dx from 0 to 1 specifying x direction to go in
         * @param dy from 0 to 1 specifying y direction go to in
         * @param action {0, 1, 2} meaning, none, split, feed
         */
        void take_action(float dx, float dy, int action) {
          auto &player = engine.player(pid);

          auto target_x = player.x() + dx * 10;
          auto target_y = player.y() + dy * 10;

          player.action = static_cast<agario::action>(action);
          player.target = agario::Location(target_x, target_y);
        }

        /* Resets the environment by resetting the game engine. */
        void reset() {
          engine.reset();
          pid = engine.template add_player<Player>("agent");
          add_bots();
          _done = false;

          // the following loop is needed to "initialize" the observation object
          // with the newly reset state so that a call to get_state directly
          // after reset will return a state representing the fresh beginning
          auto &player = engine.player(pid);
          for (int i = 0; i < ticks_per_step(); i++)
            this->_partial_observation(player, i);
        }

        bool done() const { return _done; }

        int ticks_per_step() const { return _ticks_per_step; }

        virtual void render() { };

      protected:
        Engine<renderable> engine;
        agario::pid pid;

        const int _ticks_per_step;
        const int num_bots;
        const agario::time_delta step_dt;

        bool _done;

        // override this to allow environment to get it's state from
        // intermediate frames between the start and end of a "step"
        virtual void _partial_observation(Player &player, int tick_index) { };

        // adds the specified number of bots to the game
        void add_bots() {
          for (int i = 0; i < num_bots / 2; i++) {
            engine.template add_player<HungryBot>("hungry");
            engine.template add_player<HungryShyBot>("shy");
          }
        }

      };

  } // namespace env
} // namespace agario
