#pragma once

#include "engine/GameState.hpp"

namespace agario::env::full {

  class Observation {
    typedef GameState<false> GameState;
  public:
    explicit Observation(const Player<false> &player, const GameState &game_state) {
      int num_cells = player.cells.size();
      _data[0] = new float[num_cells * 5];
    }

    const std::vector<float *> &data() const { return _data; }

    ~Observation() { }

  private:
    std::vector<float *> _data;
    std::vector<int> _sizes;
  };


  class Environment {
    static constexpr bool renderable = false;
    typedef agario::Player<renderable> Player;
    typedef agario::Cell<renderable> Cell;
    typedef agario::Pellet<renderable> Pellet;
    typedef agario::Food<renderable> Food;
    typedef agario::Virus<renderable> Virus;

    typedef agario::bot::HungryBot<renderable> HungryBot;
    typedef agario::bot::HungryShyBot<renderable> HungryShyBot;

  public:

    explicit Environment(int frames_per_step) :
      engine(), _num_frames(frames_per_step), _done(false), step_dt(DEFAULT_DT) {
      pid = engine.template add_player<Player>("agent");
    }

    reward step() {
      auto &player = engine.player(pid);

      auto mass_before = player.mass();
      for (int i = 0; i < _num_frames; i++) {
        if (player.dead()) {
          _done = true;
          break;
        }
        engine.tick(step_dt);
      }

      auto reward = player.mass() - mass_before;
      return reward;
    }

    const Observation get_state() const {
      return Observation(engine.get_game_state());
    }

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
    Engine <renderable> engine;
    agario::pid pid;

    int _num_frames;
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