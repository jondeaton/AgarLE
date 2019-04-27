#pragma once

#include <engine/Engine.hpp>
#include <core/types.hpp>
#include <core/Entities.hpp>
#include <core/Ball.hpp>
#include <bots/bots.hpp>

namespace agario::environment {

  typedef double reward;

  template<unsigned W, unsigned H, unsigned NumFrames>
  class Observation {
  public:
    unsigned char frame_data[NumFrames * W * H * 3];
  };

  template<bool renderable, unsigned W, unsigned H, unsigned NumFrames>
  class Environment {
    typedef agario::Player<renderable> Player;
    typedef agario::Cell<renderable> Cell;
    typedef agario::Pellet<renderable> Pellet;
    typedef agario::Food<renderable> Food;
    typedef agario::Virus<renderable> Virus;

    typedef agario::bot::HungryBot<renderable> HungryBot;
    typedef agario::bot::HungryShyBot<renderable> HungryShyBot;

    typedef Observation<W, H, NumFrames> Observation;

  public:

    explicit Environment(int frames_per_step) :
      frames_per_step(frames_per_step), _done(false), step_dt(1.0 / 60) {
      pid = engine.template add_player<Player>("agent");
    }

    reward step() {
      auto &player = engine.player(pid);
      auto mass_before = player.mass();
      for (int i = 0; i < frames_per_step; i++) {
        if (player.dead()) {
          _done = true;
          break;
        }
        engine.tick(step_dt);
        _store_observation(i);
      }
      return player.mass() - mass_before;
    }

    const Observation &get_state() const { return _observation; }

    void render() {}

    void take_action(float target_x, float target_y, int action) {
      auto &player = engine.player(pid);
      player.action = static_cast<agario::action>(action);
      player.target = agario::Location(target_x, target_y);
    }

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
    Observation _observation;

    void add_bots() {
      for (int i = 0; i < 10; i++)
        engine.template add_player<HungryBot>("hungry");

      for (int i = 0; i < 25; i++)
        engine.template add_player<HungryShyBot>("shy");
    }


    void _store_observation(int frame) {
      for (int i = 0; i < W * H * 3; i++)
        _observation.frame_data[frame * (W * H * 3) + i] = frame;

      // todo: render the screen into frame_data
    }

  };
}




