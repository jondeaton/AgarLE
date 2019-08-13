#pragma once

#include <agario/engine/Engine.hpp>
#include <agario/core/types.hpp>
#include <agario/core/Entities.hpp>
#include <agario/core/Ball.hpp>
#include <agario/bots/bots.hpp>
#include "agario/engine/GameState.hpp"
#include <agario/core/settings.hpp>

#include "environment/envs/BaseEnvironment.hpp"

#define DEFAULT_NUM_FOODS 10

#ifdef RENDERABLE

#include "core/renderables.hpp"
#include "rendering/window.hpp"
#include "rendering/renderer.hpp"

#endif

namespace agario {
  namespace env {

    template<bool renderable, int cell_limit, int num_foods>
    class RamObservation {
      using GameState = GameState<renderable>;
      using Player = Player<renderable>;
      using Cell = Cell<renderable>;
      using Pellet = Pellet<renderable>;
      using Virus = Virus<renderable>;
      using Food = Food<renderable>;

    public:
      using dtype = float;

      /* Construct a ram observation from the perspective of `player` for the
       * given game state `game_state`.
       */
      explicit RamObservation(const Player &player, const GameState &game_state,
                              int num_pellets, int num_viruses) {
        _make_shapes(game_state, num_pellets, num_viruses);

        // todo: we can get rid of this dynamic memory allocation
        _data = new dtype[length()];
        std::fill(_data, _data + length(), 0);

        _data[0] = game_state.ticks;
        _data[1] = game_state.arena_width;
        _data[2] = game_state.arena_height;

        int index = 3;
        index = _store_player(player, index);

        for (auto &pair : game_state.players) {
          if (*pair.second != player)
            index = _store_player(*pair.second, index);
        }

        index = _store_entities<Pellet>(game_state.pellets, index, num_pellets);
        index = _store_entities< Virus>(game_state.viruses, index, num_viruses);
        index = _store_entities<  Food>(game_state.foods,   index,   num_foods);
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
      RamObservation(RamObservation &&obs) noexcept : _data(std::move(obs._data)),
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

      ~RamObservation() { delete[] _data; }

    private:
      dtype *_data;
      std::vector<int> _shape;
      std::vector<ssize_t> _strides;

      /* crates the shape and strides to represent the multi-dimensional array */
      void _make_shapes(const GameState &state, int num_pellets, int num_viruses) {
        auto length = 1 + 2; // ticks, arena_width, arena_height
        length += 5 * state.players.size();
        length += 2 * num_pellets;
        length += 2 * num_viruses;
        length += 2 * num_foods; // not state.foods.size()

        _shape = { length };
        _strides = {(long) sizeof(dtype)};
      }

      /* stores the player's cell data into the data array */
      int _store_player(const Player &player, int start_index) {
        int cell_count = std::min<int>(cell_limit, player.cells.size());
        for (int i = 0; i < cell_count; i++) {
          auto &cell = player.cells[i];
          int index = start_index + 5 * i;

          _data[index + 0] = cell.mass();
          _data[index + 1] = cell.x;
          _data[index + 2] = cell.y;
          _data[index + 3] = cell.velocity.dx;
          _data[index + 4] = cell.velocity.dy;
        }
        return start_index + cell_count;
      }

      /* store the given entities in the data array at layer */
      template<typename U>
      int _store_entities(const std::vector<U> &entities, int start_index, int n) {

        int num_stored = 0;
        for (auto &entity : entities) {
          auto index = start_index + 2 * num_stored;

          _data[index + 0] = entity.x;
          _data[index + 1] = entity.y;

          if (++num_stored == n)
            break;
        }
        return start_index + 2 * n;
      }

    };

    template<bool renderable>
    class RamEnvironment : public BaseEnvironment<renderable> {
      using Player = agario::Player<renderable>;

      static constexpr int cell_limit = PLAYER_CELL_LIMIT;
      static constexpr int num_foods = DEFAULT_NUM_FOODS;
      using Observation = RamObservation<renderable, cell_limit, num_foods>;

    public:
      using dtype = typename Observation::dtype;

      typedef BaseEnvironment <renderable> Super;
      explicit RamEnvironment(int frames_per_step, int arena_size, bool pellet_regen,
                              int num_pellets, int num_viruses, int num_bots) :
        Super(frames_per_step, arena_size, pellet_regen, num_pellets, num_viruses, num_bots),
        num_pellets(num_pellets), num_viruses(num_viruses) {

        /* I would use if constexpr from C++17 here but that's not an option */
#ifdef RENDERABLE
        window = std::make_shared<Window>("Agar.io Environment", 512, 512);
        renderer = std::make_unique<agario::Renderer>(window,
                                                      this->engine.arena_width(),
                                                      this->engine.arena_height());
#endif
      }

      /* returns the length of the observation data... assumes  */
      int observation_length() const {
        auto length = 1 + 2; // ticks, arena_width, arena_height
        length += 5 * (1 + this->num_bots_);
        length += 2 * num_pellets;
        length += 2 * num_viruses;
        length += 2 * num_foods;
        return length;
      }

      /**
       * Returns the current state of the world (without advancing through time)
       * @return An Observation object representing the "RAM" of the game
       */
      Observation get_state() const {
        auto &player = this->engine_.get_player(this->pid);
        auto &game_state = this->engine_.get_game_state();

        Observation observation(player, game_state, num_pellets, num_viruses);
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
      int num_pellets, num_viruses;

#ifdef RENDERABLE
      std::unique_ptr<agario::Renderer> renderer;
      std::shared_ptr<Window> window;
#endif
    };

  } // namespace env
} // namespace agario
