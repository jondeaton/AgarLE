#pragma once

#include <engine/Engine.hpp>
#include <core/types.hpp>
#include <core/Entities.hpp>
#include <core/Ball.hpp>
#include <bots/bots.hpp>

#include "engine/GameState.hpp"
#include "envs/BaseEnvironment.hpp"

#define DEFAULT_GRID_SIZE 128

#ifdef RENDERABLE

#include "core/renderables.hpp"
#include "rendering/window.hpp"
#include "rendering/renderer.hpp"
static bool _renderable = true;

#else
static bool _renderable = false;
#endif

namespace agario {
  namespace env {

    template<typename T, bool renderable>
    class GridObservation {
      typedef GameState<renderable> GameState;
      typedef Player<renderable> Player;
      typedef Cell<renderable> Cell;
      typedef Pellet<renderable> Pellet;
      typedef Virus<renderable> Virus;
      typedef Food<renderable> Food;

    public:
      /* Construct a grid observation from the perspective of `player` for the 
       * given game state `game_state`. Specify the size of the grid whether
       * to store cells, others, viruses and pellets with the remaining arguments
       */
      explicit GridObservation(const Player &player, const GameState &game_state,
        int grid_size, bool cells, bool others, bool viruses, bool pellets) :
        _grid_size(grid_size), _view_size(50) , _state(game_state) {
        // todo: change view size depending on player

        _make_shapes(grid_size, cells, others, viruses, pellets);
        _data = new T[length()];

        int i = 0;
        if (pellets) {
          _store_entities<Pellet>(game_state.pellets, player, i);
          i++;
        }

        if (viruses) {
          _store_entities<Virus>(game_state.viruses, player, i);
          i++;
        }

        if (cells) {
          _store_entities<Cell>(player.cells, player, i);
          i++;
        }

        if (others) {
          for (auto &pair : game_state.players) {
            auto &other_player = *pair.second;
            _store_entities<Cell>(other_player.cells);
          }
          i++;
        }
      }
      
      /* data buffer, mulit-dim array shape and sizes*/
      const std::int32_t * &data() const { return _data; }
      std::vector<int> shape() const { return _shape; }
      std::vector<ssize_t> strides () const { return _strides; }
      
      /* full length of data array */
      int length() const {
        int len = 1;
        for (auto s : _shape)
          len *= s;
        return len;
      }

      // todo: move, copy constructors/assignment

      ~GridObservation() {
        delete[] _data;
      }

    private:
      std::uint8_t *_data;
      std::vector<int> _shape;
      std::vector<ssize_t> _strides;
      int _view_size;
      int _grid_size;
      const GameState &_state;

      /* crates the shape and strides to represent the multi-dimensional array */
      void _make_shapes (int grid_size, bool cells, bool others, bool viruses, bool pellets) {
        int num_layers = static_cast<int>(cells + others + viruses + pellets);
        _shape = { num_layers, grid_size, grid_size };
        _strides = {
          grid_size * grid_size * sizeof(T),
                      grid_size * sizeof(T),
                                  sizeof(T)
        };
      }

      /* store the given entities in the data array at layer  */
      template<typename U>
      void _store_entities(const std::vector<U> &entities, const Player &player, int channel) {
        int grid_x, grid_y;
        for (auto &entity : entities) {
          _world_to_grid (entity.location() - player.location(), grid_x, grid_y);
          if (0 <= grid_x && grid_x < _grid_size && 0 <= grid_y && grid_y < _grid_size)
            _data[_index(channel, grid_x, grid_y)] = entity.mass();
        }
      }

      void _add_out_of_bounds(const Player &player, int channel) {
        int centering = _grid_size / 2;
        for (int i = 0; i < _grid_size; i++)
          for (int j = 0; j < _grid_size; j++) {
            auto loc = _grid_to_world (i, j);
            if (0 > loc.x || loc.x > _state.arena_width || loc.y < 0 || loc.y > _state.arena_height)
              _data[_index(channel, i, j)] = -1;
          }
      }

      /* converts world coordinates to grid coordinates */
      void _world_to_grid (const Location &loc, int &grid_x, int &grid_y) {
        int centering = _grid_size / 2;
        grid_x = static_cast<int>(_grid_size * loc.x / _view_size) + centering;
        grid_y = static_cast<int>(_grid_size * loc.y / _view_size) + centering;
      }

      Location _grid_to_world (int grid_x, int grid_y) {
        int centering = _grid_size / 2;

        auto x_diff = grid_x - centering;
        auto y_diff = grid_y - centering;

        auto x_loc = x_diff * _view_size / _grid_size;
        auto y_loc = y_diff * _view_size / _grid_size;
        return Location(x_loc, y_loc);
      }

      int _index (int c, int x, int y) {
        return _strides[0] * c + _strides[1] * x + _strides[2] * y;
      }
    };


    template<bool renderable>
    class GridEnvironment : public BaseEnvironment<renderable> {
      typedef agario::Player<renderable> Player;
      typedef GridObservation<int, renderable> GridObservation;

    public:
      typedef BaseEnvironment<renderable> Super;

      explicit GridEnvironment(int frames_per_step, int arena_size, bool pellet_regen,
                               int num_pellets, int num_viruses, int num_bots) :
                               _grid_size(DEFAULT_GRID_SIZE),
                               _observe_cells(true),   _observe_others(true),
                               _observe_viruses(true), _observe_pellets(true),
        Super(frames_per_step, arena_size, pellet_regen, num_pellets, num_viruses, num_bots) {

        /* I would use if constexpr from C++17 here but that's not an option */
#ifdef RENDERABLE
        window = std::make_shared<Window>("Agar.io Environment", 512, 512);
        renderer = std::make_unique<agario::Renderer>(window,
                                                      this->engine.arena_width(),
                                                      this->engine.arena_height());
#endif
      }

      /* Configures the observation types that will be returned. */
      void configure_observation(int grid_size, bool cells, bool others, bool viruses, bool pellets) {
        _grid_size = grid_size;
        _observe_cells = cells;
        _observe_others = others;
        _observe_viruses = viruses;
        _observe_pellets = pellets;
      }

      /**
       * Returns the current state of the world without
       * advancing through time
       * @return An Observation object containing all of the
       * locations of every entity in the current state of the game world
       */
      const GridObservation get_state() const {
        auto &player = this->engine.get_player(this->pid);
        return GridObservation(player, this->engine.get_game_state(),
          _grid_size, _observe_cells, _observe_others, _observe_viruses, _observe_pellets);
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
      int _grid_size;
      bool _observe_cells;
      bool _observe_others;
      bool _observe_viruses;
      bool _observe_pellets;

#ifdef RENDERABLE
      std::unique_ptr<agario::Renderer> renderer;
      std::shared_ptr<Window> window;
#endif
    };

  } // namespace env
} // namespace agario
