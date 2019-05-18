#pragma once

#include <engine/Engine.hpp>
#include <core/types.hpp>
#include <core/Entities.hpp>
#include <core/Ball.hpp>
#include <bots/bots.hpp>

#include "engine/GameState.hpp"

#define DEFAULT_DT (1.0 / 60)

namespace agario {
  namespace env {

      typedef double reward;

      template<bool renderable>
      class Observation { };

      template<bool renderable>
      class BaseEnvironment {
        typedef agario::Player<renderable> Player;
        typedef Observation<renderable> Observation;
        typedef agario::bot::HungryBot<renderable> HungryBot;
        typedef agario::bot::HungryShyBot<renderable> HungryShyBot;

      public:

        explicit BaseEnvironment(unsigned frames_per_step, unsigned arena_size, bool pellet_regen,
                             unsigned num_pellets, unsigned num_viruses, unsigned num_bots) :
          engine(arena_size, arena_size, num_pellets, num_viruses, pellet_regen),
          _num_frames(frames_per_step), _num_bots(num_bots), _done(false),
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
          for (int i = 0; i < _num_frames; i++) {
            if (player.dead()) {
              _done = true;
              break;
            }
            engine.tick(step_dt);
            _partial_observation(player, i);
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

        /**
         * Resets the environment by resetting the game enging
         */
        void reset() {
          engine.reset();
          pid = engine.template add_player<Player>("agent");
          add_bots();
          _done = false;
        }

        bool done() const { return _done; }

      protected:
        Engine<renderable> engine;
        agario::pid pid;
        int _num_frames;
        int _num_bots;
        bool _done;
        std::chrono::duration<float> step_dt;

        // override this to allow environment to get it's state from
        // intermediate frames between the start and end of a "step"
        virtual void _partial_observation(Player &player, int frame) { };

        // adds the specified number of bots to the game
        void add_bots() {
          for (int i = 0; i < _num_bots / 2; i++) {
            engine.template add_player<HungryBot>("hungry");
            engine.template add_player<HungryShyBot>("shy");
          }
        }

      };

  } // namespace env
} // namespace agario
