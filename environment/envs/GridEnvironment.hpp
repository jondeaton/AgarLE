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
        int grid_size, bool cells, bool others, bool viruses, bool pellets) {

        _make_shapes(grid_size, cells, others, viruses, pellets);
        _data = new T[length()];

        _view_size = 50; // todo: change view size depending on player

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

      /*
       * store the given entities in the data array at layer 
       */
      template<typename U>
      void _store_entities(const std::vector<U> &entities, const Player &player, int i) {
        int num = entities.size();
        int index = i * _strides[0];
        float box_size = _view
        for (auto &entity : entities) {
          auto loc_rel = entity.location() - player.location();
          int grid_x = loc_rel.x 

        }


        /*
         for entity in entities:
            grid_x = int((entity[0] - loc[0]) / self.box_size) + self.grid_size // 2
            grid_y = int((entity[1] - loc[1]) / self.box_size) + self.grid_size // 2
            value = 1 if len(entity) <= 2 else entity[-1]

            if 0 <= grid_x < self.grid_size and 0 <= grid_y < self.grid_size:
                entity_features[grid_x][grid_y] += value

                */
      }

      void _add_out_of_bounds(const Player &player, int i) {
        /*
         """ adds sentinel_value to out of bounds locations """
        for i in range(self.grid_size):
            for j in range(self.grid_size):
                x_diff = i - int(self.grid_size / 2)
                y_diff = j - int(self.grid_size / 2)

                x_loc = x_diff * self.box_size + loc[0]
                y_loc = y_diff * self.box_size + loc[1]

                # if in bounds, increment
                if not (0 <= x_loc < self.arena_size and 0 <= y_loc < self.arena_size):
                    entity_features[i][j] = sentinel_value

         */

      }

      template<typename T>
      void _copy_entities(const std::vector<T> &entities, float *buffer) {
        int i = 0;
        for (auto &e : entities) {
          buffer[i * 2 + 0] = static_cast<float>(e.x);
          buffer[i * 2 + 1] = static_cast<float>(e.y);
          i++;
        }
      }

      // Cell specialization
      void _copy_entities(const std::vector<Cell> &cells, float *buffer) {
        int i = 0;
        for (auto &cell : cells) {
          buffer[i * 5 + 0] = (float) cell.x;
          buffer[i * 5 + 1] = (float) cell.y;
          buffer[i * 5 + 2] = (float) cell.velocity.dx;
          buffer[i * 5 + 3] = (float) cell.velocity.dy;
          buffer[i * 5 + 4] = (float) cell.mass();
          i++;
        }
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
