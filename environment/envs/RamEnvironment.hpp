#pragma once

#include <agario/engine/Engine.hpp>
#include <agario/core/types.hpp>
#include <agario/core/Entities.hpp>
#include <agario/core/Ball.hpp>
#include <agario/bots/bots.hpp>
#include "agario/engine/GameState.hpp"

#include "environment/envs/BaseEnvironment.hpp"

#define DEFAULT_GRID_SIZE 128
#define VIEW_SIZE 30

#ifdef RENDERABLE

#include "core/renderables.hpp"
#include "rendering/window.hpp"
#include "rendering/renderer.hpp"

#endif

namespace agario {
  namespace env {

    template<bool renderable>
    class RamObservation {
      typedef GameState<renderable> GameState;
      typedef Player<renderable> Player;
      typedef Cell<renderable> Cell;
      typedef Pellet<renderable> Pellet;
      typedef Virus<renderable> Virus;
      typedef Food<renderable> Food;

    public:
      using dtype = float;

      /* Construct a ram observation from the perspective of `player` for the
       * given game state `game_state`.
       */
      explicit RamObservation(const Player &player, const GameState &game_state) {
        _make_shapes(game_state);

        _data = new dtype[length()];

        _data[0] = game_state.ticks;
        _data[1] = game_state.arena_width;
        _data[2] = game_state.arena_height;

        // todo; need to limit the number of Cells that a player can have!

        int index = 3;
        index = _store_player(player, index);

        for (auto &pair : game_state.players) {
          if (*pair.second != player)
            index = _store_player(player, index);
        }

        index = _store_entities<Pellet>(game_state.pellets, index);
        index = _store_entities<Virus>(game_state.viruses, index);
        index = _store_entities<Food>(game_state.foods, index);
      }

      /* data buffer, mulit-dim array shape and sizes*/
      const dtype *data() const { return _data; }
      std::vector<int> shape() const { return _shape; }
      std::vector<ssize_t> strides() const { return _strides; }

      /* full length of data array */
      int length() const { return _strides[0]; }

      RamObservation(const RamObservation &) = delete; // no copy constructor
      RamObservation &operator=(const RamObservation &) = delete; // no copy assignments

      /* move constructor */
      RamObservation(RamObservation &&obs) noexcept :
        _data(std::move(obs._data)),
        _shape(std::move(obs._shape)),
        _strides(std::move(obs._strides)) {
        obs._data = nullptr;
      };

      /* move assignment */
      RamObservation &operator=(RamObservation &&obs) noexcept {
        _data = std::move(obs._data);
        _shape = std::move(obs._shape);
        _strides = std::move(obs._strides);
        obs._data = nullptr;
        return *this;
      };

      ~RamObservation() {
        delete[] _data;
      }

    private:
      dtype *_data;
      std::vector<int> _shape;
      std::vector<ssize_t> _strides;

      /* crates the shape and strides to represent the multi-dimensional array */
      void _make_shapes(const GameState &state) {
        auto length = 1 + 2; // ticks, arena_width, arena_height
        length += 5 * state.players.size();
        length += 2 * state.pellets.size();
        length += 2 * state.viruses.size();
        length += 2 * state.foods.size();

        _shape = { length };
        _strides = { (long) sizeof(dtype) };
      }

      int _store_player(const Player &player, int index) {
        int _index = index
        for (auto &cell : player.cells) {
          _data[index + 0] = cell.mass();
          _data[index + 1] = cell.x;
          _data[index + 2] = cell.y;
          _data[index + 3] = cell.velocity().dx;
          _data[index + 4] = cell.velocity().dy;
          index += 5;
        }
//        return _index + 5 * Player::max_cells;
      }

      /* store the given entities in the data array at layer */
      template<typename U>
      int _store_entities(const std::vector<U> &entities, const Player &player, int index) {
        for (auto &entity : entities) {

        }
      }

    };


    template<bool renderable>
    class RamEnvironment : public BaseEnvironment<renderable> {
      typedef agario::Player<renderable> Player;
      typedef RamObservation<renderable> RamObservation;

    public:
      typedef BaseEnvironment<renderable> Super;

      explicit RamEnvironment(int frames_per_step, int arena_size, bool pellet_regen,
                               int num_pellets, int num_viruses, int num_bots) :
        Super(frames_per_step, arena_size, pellet_regen, num_pellets, num_viruses, num_bots) {

        /* I would use if constexpr from C++17 here but that's not an option */
#ifdef RENDERABLE
        window = std::make_shared<Window>("Agar.io Environment", 512, 512);
        renderer = std::make_unique<agario::Renderer>(window,
                                                      this->engine.arena_width(),
                                                      this->engine.arena_height());
#endif
      }

      /**
       * Returns the current state of the world without
       * advancing through time
       * @return An Observation object containing all of the
       * locations of every entity in the current state of the game world
       */
      RamObservation get_state() const {
        auto &player = this->engine.get_player(this->pid);
        RamObservation observation(player, this->engine.get_game_state());
        return observation; // return-value-optimization
      }

      void render() {
#ifdef RENDERABLE
        auto &player = this->engine.player(this->pid);
        renderer->render_screen(player, this->engine.game_state());

        glfwPollEvents();
        window->swap_buffers();
#endif
      }

    private:

#ifdef RENDERABLE
      std::unique_ptr<agario::Renderer> renderer;
      std::shared_ptr<Window> window;
#endif
    };

  } // namespace env
} // namespace agario
