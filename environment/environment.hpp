#pragma once

#include <engine/Engine.hpp>
#include <core/types.hpp>
#include <core/Entities.hpp>
#include <core/Ball.hpp>
#include <bots/bots.hpp>

namespace agario::environment {

    typedef double reward;

    class Action {
    public:
      agario::Location target;
      agario::action game_action;
    };


    class Observation {
    public:
      // todo
    };

    template<bool renderable>
    class Environment {
      typedef agario::Player<renderable> Player;
      typedef agario::Cell<renderable> Cell;
      typedef agario::Pellet<renderable> Pellet;
      typedef agario::Food<renderable> Food;
      typedef agario::Virus<renderable> Virus;

      typedef agario::bot::HungryBot<renderable> HungryBot;
      typedef agario::bot::HungryShyBot<renderable> HungryShyBot;

    public:

      Environment(int frames_per_step) :
      frames_per_step(frames_per_step), _done(false), step_dt(1.0 / 60) {
        pid = engine.template add_player<Player>("agent");
      }

      reward step(Action action) {
        for (int i = 0; i < frames_per_step; i++) {
          auto &player = engine.player(pid);
          if (player.dead()) {
            _done = true;
            break;
          }
          engine.tick(step_dt);
        }
      }

      Observation get_state() const {}

      void render() {}

      void take_action(Action action) {
        auto &player = engine.player(pid);
        player.action = action.game_action;
        player.target = action.target;
      }

      reward get_reward() const {}

      void reset() {
        engine.reset();
        pid = engine.template add_player<Player>("agent");
        add_bots();
        _done = false;
      }

      bool done() const { return _done; }

    private:
      Engine<renderable> engine;
      agario::pid pid;
      int frames_per_step;
      bool _done;
      std::chrono::duration<float> step_dt;

      void add_bots() {
        for (int i = 0; i < 10; i++)
          engine.template add_player<HungryBot>("hungry");

        for (int i = 0; i < 25; i++)
          engine.template add_player<HungryShyBot>("shy");
      }

    };
}




