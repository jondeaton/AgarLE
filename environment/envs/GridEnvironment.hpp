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

    // maybe?
//    enum Observe : unsigned char {
//      cells   = 1 << 0,
//      others  = 1 << 1,
//      viruses = 1 << 2,
//      pellets = 1 << 3,
//      foods   = 1 << 4,
//    };


    template<typename T, bool renderable>
    class GridObservation {
      using GameState = GameState<renderable>;
      using Player = Player<renderable>;
      using Cell = Cell<renderable>;
      using Pellet = Pellet<renderable>;
      using Virus = Virus<renderable>;
      using Food = Food<renderable>;

    public:
      typedef T dtype;

      explicit GridObservation() : _data(nullptr) { }

      /* Construct a grid observation from the perspective of `player` for the 
       * given game state `game_state`. Specify the size of the grid whether
       * to store cells, others, viruses and pellets with the remaining arguments
       */
      explicit GridObservation(int num_frames, int grid_size, bool cells, bool others, bool viruses, bool pellets) :
        _grid_size(grid_size), _view_size(VIEW_SIZE),
        _num_frames(num_frames),
        _observe_cells(cells), _observe_others(others), _observe_viruses(viruses), _observe_pellets(pellets) {
        _make_shapes();
        _data = new dtype[length()];
        std::fill(_data, _data + length(), 0);
      }

      /* configures the observation to store a particular */
      void configure(int num_frames, int grid_size, bool c, bool o, bool v, bool p) {
        _num_frames = num_frames;
        _grid_size = grid_size;
        _observe_cells = c;
        _observe_others = o;
        _observe_viruses = v;
        _observe_pellets = p;

        delete[] _data;

        _make_shapes();
        _data = new dtype[length()];
        std::fill(_data, _data + length(), 0);
      }

      /* data buffer, mulit-dim array shape and sizes*/
      const dtype *data() const { return _data; }
      std::vector<int> shape() const {
        if (_data == nullptr)
          throw EnvironmentException("GridObservation was not configured.");
        return _shape;
      }
      std::vector<ssize_t> strides() const {
        if (_data == nullptr)
          throw EnvironmentException("GridObservation was not configured.");
        return _strides;
      }

      /* adds a single frame to the observation at index `frame_index` */
      void add_frame(const Player &player, const GameState &game_state, int frame_index) {
        if (_data == nullptr)
          throw EnvironmentException("GridObservation was not configured.");

        int channel = channels_per_frame() * frame_index;
        _mark_out_of_bounds(player, channel, game_state.arena_width, game_state.arena_height);

        if (_observe_pellets) {
          channel++;
          _store_entities<Pellet>(game_state.pellets, player, channel);
        }

        if (_observe_viruses) {
          channel++;
          _store_entities<Virus>(game_state.viruses, player, channel);
        }

        if (_observe_cells) {
          channel++;
          _store_entities<Cell>(player.cells, player, channel);
        }

        if (_observe_others) {
          channel++;
          for (auto &pair : game_state.players) {
            Player &other_player = *pair.second;
            _store_entities<Cell>(other_player.cells, player, channel);
          }
        }
      }

      /* full length of data array */
      int length() const {
        int len = 1;
        for (auto s : _shape)
          len *= s;
        return len;
      }

      // no copy operations because if you're copying this object then
      // you're probably not using it correctly
      GridObservation(const GridObservation &) = delete; // no copy constructor
      GridObservation &operator=(const GridObservation &) = delete; // no copy assignments

      /* move constructor */
      GridObservation(GridObservation &&obs) noexcept :
        _data(std::move(obs._data)), _shape(std::move(obs._shape)),
        _strides(std::move(obs._strides)), _view_size(std::move(obs._view_size)),
        _grid_size(std::move(obs._grid_size)) {
        obs._data = nullptr;
      };

      /* move assignment */
      GridObservation &operator=(GridObservation &&obs) noexcept {
        _data = std::move(obs._data);
        _shape = std::move(obs._shape);
        _strides = std::move(obs._strides);
        _view_size = std::move(obs._view_size);
        _grid_size = std::move(obs._grid_size);
        obs._data = nullptr;
        return *this;
      };

      ~GridObservation() {
        delete[] _data;
      }

    private:
      T *_data;
      std::vector<int> _shape;
      std::vector<ssize_t> _strides;
      int _view_size;
      int _grid_size;

      int _num_frames;
      bool _observe_pellets;
      bool _observe_cells;
      bool _observe_viruses;
      bool _observe_others;

      /* the number of channels in each frame */
      int channels_per_frame() const {
        return static_cast<int>(1 + _observe_cells + _observe_others + _observe_viruses + _observe_pellets);
      }

      /* crates the shape and strides to represent the multi-dimensional array */
      void _make_shapes() {
        int num_channels = _num_frames * channels_per_frame();
        _shape = { num_channels, _grid_size, _grid_size };

        auto dtype_size = static_cast<long>(sizeof(dtype));
        _strides = {
          _grid_size * _grid_size * dtype_size,
          _grid_size * dtype_size,
          dtype_size
        };
      }

      /* store the given entities in the data array at layer */
      template<typename U>
      void _store_entities(const std::vector<U> &entities, const Player &player, int channel) {
        int grid_x, grid_y;
        for (auto &entity : entities) {
          _world_to_grid(entity.location() - player.location(), grid_x, grid_y);

          int index = _index(channel, grid_x, grid_y);
          if (_inside_grid(grid_x, grid_y))
            _data[index] = entity.mass();
        }
      }

      void _mark_out_of_bounds(const Player &player, int channel, int arena_width, int arena_height) {
        int centering = _grid_size / 2;
        for (int i = 0; i < _grid_size; i++)
          for (int j = 0; j < _grid_size; j++) {

            auto loc = _grid_to_world(i, j);
            int index = _index(channel, i, j);
            bool in_bounds = 0 <= loc.x && loc.x < arena_width && 0 <= loc.y && loc.y < arena_height;
            _data[index] = in_bounds ? 0 : -1;

          }
      }

      /* converts world coordinates to grid coordinates */
      void _world_to_grid(const Location &loc, int &grid_x, int &grid_y) {
        int centering = _grid_size / 2;
        grid_x = static_cast<int>(_grid_size * loc.x / _view_size) + centering;
        grid_y = static_cast<int>(_grid_size * loc.y / _view_size) + centering;
      }

      Location _grid_to_world(int grid_x, int grid_y) {
        int centering = _grid_size / 2;

        auto x_diff = grid_x - centering;
        auto y_diff = grid_y - centering;

        auto x_loc = x_diff * _view_size / _grid_size;
        auto y_loc = y_diff * _view_size / _grid_size;
        return Location(x_loc, y_loc);
      }

      int _index(int c, int x, int y) {
        int channel_stride = _grid_size * _grid_size;
        int x_stride = _grid_size;
        int y_stride = 1;
        return channel_stride * c + x_stride * x + y_stride * y;
      }

      bool _inside_grid(int grid_x, int grid_y) {
        return 0 <= grid_x && grid_x < _grid_size && 0 <= grid_y && grid_y < _grid_size;
      }
    };


    template<typename T, bool renderable>
    class GridEnvironment : public BaseEnvironment<renderable> {
      typedef agario::Player<renderable> Player;
      typedef GridObservation<T, renderable> GridObservation;

    public:
      typedef T dtype;
      typedef BaseEnvironment<renderable> Super;

      explicit GridEnvironment(int frames_per_step, int arena_size, bool pellet_regen,
                               int num_pellets, int num_viruses, int num_bots) :
        _grid_size(DEFAULT_GRID_SIZE),
        _observe_cells(true), _observe_others(true),
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
      void configure_observation(int grid_size, bool observe_cells, bool observe_others,
                                 bool observe_viruses, bool observe_pellets) {
        observation.configure(this->frames_per_step(), grid_size, observe_cells, observe_others, observe_viruses, observe_pellets);
      }

      std::vector<int> observation_shape() const {
        return observation.shape();
      }

      /**
       * Returns the current state of the world without advancing through time
       * @return An Observation object containing all of the
       * locations of every entity in the current state of the game world
       */
      const GridObservation &get_state() const { return observation; }

      /* allows for intermediate grid frames to be stored in the GridObservation */
      void _partial_observation(Player &player, int frame_index) override {
        auto &state = this->engine.game_state();
        observation.add_frame(player, state, frame_index);
      }

      void render() override {
#ifdef RENDERABLE
        auto &player = this->engine.player(this->pid);
        renderer->render_screen(player, this->engine.game_state());

        glfwPollEvents();
        window->swap_buffers();
#endif
      }

    private:
      GridObservation observation;
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
