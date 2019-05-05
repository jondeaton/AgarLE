#pragma once

#include <engine/Engine.hpp>
#include <core/types.hpp>
#include <core/Entities.hpp>
#include <core/Ball.hpp>
#include <bots/bots.hpp>

#include "engine/GameState.hpp"

#define DEFAULT_DT (1.0 / 60)

namespace agario::env::full {

  typedef double reward;

  template <bool renderable>
  class Observation {
    typedef GameState<renderable> GameState;
    typedef Player<renderable> Player;
  public:
    explicit Observation(const Player &player, const GameState &game_state) {
      // todo: please refactor :(

      int num_viruses = game_state.viruses.size();
      _data.push_back(new float[2 * num_viruses]);
      _store_viruses(game_state, _data[0]);
      _shapes.push_back({ num_viruses, 2 });

      int num_foods = game_state.foods.size();
      _data.push_back(new float[2 * num_foods]);
      _store_foods(game_state, _data[1]);
      _shapes.push_back({ num_foods, 2 });

      int num_pellets = game_state.pellets.size();
      _data.push_back(new float[2 * num_pellets]);
      _store_pellets(game_state, _data[2]);
      _shapes.push_back({ num_pellets, 2 });

      int num_cells = player.cells.size();
      _data.push_back(new float[5 * num_cells]);
      _store_player_cells(player, _data[3]);
      _shapes.push_back({ num_cells, 5 });

      for (auto &pair : game_state.players) {
        auto &other_player = *pair.second;
        if (other_player == player) continue;

        num_cells = other_player.cells.size();
        auto player_data = new float[5 * num_cells];
        _store_player_cells(other_player, player_data);
        _data.push_back(player_data);
        _shapes.push_back({ num_cells, 5 });
      }
    }

    const std::vector<float *> &data() const { return _data; }

    const std::vector<std::vector<int>> shapes() const { return _shapes; }

    ~Observation() {
      for (float *d : _data)
        delete[] d;
    }

  private:
    std::vector<float *> _data;
    std::vector<std::vector<int>> _shapes;

    void _store_viruses(const GameState &game_state, float *buffer) {
      int i = 0;
      for (auto &virus : game_state.viruses) {
        buffer[i * 2 + 0] = (float) virus.x;
        buffer[i * 2 + 1] = (float) virus.y;
        i++;
      }
    }

    void _store_foods(const GameState &game_state, float *buffer) {
      int i = 0;
      for (auto &food : game_state.foods) {
        buffer[i * 2 + 0] = (float) food.x;
        buffer[i * 2 + 1] = (float) food.y;
        i++;
      }
    }

    void _store_pellets(const GameState &game_state, float *buffer) {
      int i = 0;
      for (auto &pellet : game_state.pellets) {
        buffer[i * 2 + 0] = (float) pellet.x;
        buffer[i * 2 + 1] = (float) pellet.y;
        i++;
      }
    }

    void _store_player_cells(const Player &player, float *buffer) {
      int i = 0;
      for (auto &cell : player.cells) {
        buffer[i * 5 + 0] = (float) cell.x;
        buffer[i * 5 + 1] = (float) cell.y;
        buffer[i * 5 + 2] = (float) cell.velocity.dx;
        buffer[i * 5 + 3] = (float) cell.velocity.dy;
        buffer[i * 5 + 4] = (float) cell.mass();
        i++;
      }
    }

  };

  template <bool renderable>
  class Environment {
    typedef agario::Player<renderable> Player;
    typedef Observation<renderable> Observation;
    typedef agario::bot::HungryBot<renderable> HungryBot;
    typedef agario::bot::HungryShyBot<renderable> HungryShyBot;

  public:

    explicit Environment(int frames_per_step) :
      engine(), _num_frames(frames_per_step), _done(false), step_dt(DEFAULT_DT) {
      pid = engine.template add_player<Player>("agent");
      reset();
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
      auto &player = engine.get_player(pid);
      return Observation(player, engine.get_game_state());
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
