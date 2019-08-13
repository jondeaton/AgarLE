#pragma once

#include <assert.h>

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

    template<typename T, bool renderable>
    class GridObservation {
      using GameState = GameState<renderable>;
      using Player = Player<renderable>;
      using Cell = Cell<renderable>;
      using Pellet = Pellet<renderable>;
      using Virus = Virus<renderable>;
      using Food = Food<renderable>;

    public:
      using dtype = T;

      GridObservation() : _data(nullptr) { }

      explicit GridObservation(int num_frames, int grid_size, 
          bool cells, bool others, bool viruses, bool pellets) :
        _num_frames(num_frames),
        _grid_size(grid_size),
        _observe_cells(cells), _observe_others(others), 
        _observe_viruses(viruses), _observe_pellets(pellets) {
        _make_shapes();
        _data = new dtype[length()];
        clear_data();
      }

      /* configures the observation for a particular size */
      void configure(int num_frames, int grid_size, bool observe_cells, bool observe_others,
                     bool observe_viruses, bool observe_pellets) {
        _num_frames = num_frames;
        _grid_size = grid_size;
        _observe_cells = observe_cells;
        _observe_others = observe_others;
        _observe_viruses = observe_viruses;
        _observe_pellets = observe_pellets;

        delete[] _data;

        _make_shapes();
        _data = new dtype[length()];
        clear_data();
      }

      bool configured() const  { return _data != nullptr; }

      /* data buffer, mulit-dim array shape and sizes*/
      const dtype *data() const {
        if (!configured())
          throw EnvironmentException("GridObservation was not configured.");
        return _data;
      }

      const std::vector<int> &shape() const {
        if (!configured())
          throw EnvironmentException("GridObservation was not configured.");
        return _shape;
      }

      const std::vector<ssize_t> &strides() const {
        if (!configured())
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

      void clear_data() {
        std::fill(_data, _data + length(), 0);
      }

      /* full length of data array */
      int length() const {
        int len = 1;
        for (auto s : _shape)
          len *= s;
        return len;
      }

      int num_frames() const { return _num_frames; }

      // no copy operations because if you're copying this object then
      // you're probably not using it correctly
      GridObservation(const GridObservation &) = delete; // no copy constructor
      GridObservation &operator=(const GridObservation &) = delete; // no copy assignments

      ~GridObservation() { delete[] _data; }

    private:
      dtype *_data;
      std::vector<int> _shape;
      std::vector<ssize_t> _strides;

      /* observation configuration parameters */
      int _num_frames;
      int _grid_size;
      bool _observe_pellets;
      bool _observe_cells;
      bool _observe_viruses;
      bool _observe_others;

      /* the number of channels in each frame */
      int channels_per_frame() const {
        // the +1 is for the out-of-bounds channel
        return static_cast<int>(1 + _observe_cells + _observe_others 
                                  + _observe_viruses + _observe_pellets);
      }

      /* creates the shape and strides to represent the multi-dimensional array */
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

      /* stores the given entities in the data array at the given `channel` */
      template<typename U>
      void _store_entities(const std::vector<U> &entities, const Player &player, int channel) {
        float view_size = _view_size(player);

        int grid_x, grid_y;
        for (auto &entity : entities) {
          _world_to_grid(player, entity.location(), view_size, grid_x, grid_y);

          int index = _index(channel, grid_x, grid_y);
          if (_inside_grid(grid_x, grid_y))
            _data[index] = entity.mass();
        }
      }

      /* marks out-of-bounds locations on the given `channel` */
      void _mark_out_of_bounds(const Player &player, int channel, 
          agario::distance arena_width, agario::distance arena_height) {
        float view_size = _view_size(player);

        int centering = _grid_size / 2;
        for (int i = 0; i < _grid_size; i++)
          for (int j = 0; j < _grid_size; j++) {

            auto loc = _grid_to_world(player, view_size, i, j);
            int index = _index(channel, i, j);
            bool in_bounds = _in_bounds(loc, arena_width, arena_height);
            _data[index] = in_bounds ? 0 : -1;
          }
      }

      /* determines what the view size should be, based on the player's mass */
      float _view_size(const Player &player) const {
        // todo: make this consistent with the renderer's view (somewhat tough)
        return agario::clamp<float>(2 * player.mass(), 100, 300);
      }

      /* converts world-coordinates to grid-coordinates */
      void _world_to_grid(const Player &player, const Location & loc,
        float view_size, int &grid_x, int &grid_y) const {

        float centering = _grid_size / 2.0;

        auto diff_x = loc.x - player.x();
        auto diff_y = loc.y - player.y();

        grid_x = static_cast<int>(_grid_size * diff_x / view_size + centering);
        grid_y = static_cast<int>(_grid_size * diff_y / view_size + centering);
      }

      /* converts grid-coordinates to world-coordinates */
      Location _grid_to_world(const Player &player, float view_size, int grid_x, int grid_y) const {
        float centering = _grid_size / 2.0;

        float x_diff = static_cast<float>(grid_x) - centering;
        float y_diff = static_cast<float>(grid_y) - centering;

        float dx = x_diff * view_size / _grid_size;
        float dy = y_diff * view_size / _grid_size;
        return player.location() + Location(dx, dy);
      }

      /* the index of a given channel, x, y grid-coordinate in the `_data` array */
      int _index(int channel, int grid_x, int grid_y) const {
        int channel_stride = _grid_size * _grid_size;
        int x_stride = _grid_size;
        int y_stride = 1;
        return channel_stride * channel + x_stride * grid_x + y_stride * grid_y;
      }

      /* determines whether the given x, y grid-coordinates, are within the grid */
      bool _inside_grid(int grid_x, int grid_y) const {
        return 0 <= grid_x && grid_x < _grid_size && 0 <= grid_y && grid_y < _grid_size;
      }

      bool _in_bounds(const Location &loc, agario::distance arena_width, agario::distance arena_height) {
        return 0 <= loc.x && loc.x < arena_width && 0 <= loc.y && loc.y < arena_height;
      }
    };


    template<typename T, bool renderable>
    class GridEnvironment : public BaseEnvironment<renderable> {
      using Player = agario::Player<renderable>;
      using GridObservation = GridObservation<T, renderable>;
      using Super = BaseEnvironment<renderable>;

    public:
      using dtype = T;
      using Observation = GridObservation;

      explicit GridEnvironment(int num_agents, int ticks_per_step, int arena_size, bool pellet_regen,
                               int num_pellets, int num_viruses, int num_bots) :
        Super(num_agents, ticks_per_step, arena_size, pellet_regen, num_pellets, num_viruses, num_bots) {

        /* I would use if constexpr from C++17 here but that's not an option */
#ifdef RENDERABLE
        window = std::make_shared<Window>("Agar.io Environment", 512, 512);
        renderer = std::make_unique<agario::Renderer>(window,
                                                      this->engine.arena_width(),
                                                      this->engine.arena_height());
#endif
      }

      /* Configures the observation types that will be returned. */
      void configure_observation(int num_frames, int grid_size, bool observe_cells, bool observe_others,
                                 bool observe_viruses, bool observe_pellets) {
        if (num_frames > this->ticks_per_step())
          throw EnvironmentException("num_frames may not exceed ticks-per-step");

        for (auto &observation : observations)
          observation.configure(num_frames, grid_size, observe_cells, observe_others, observe_viruses, observe_pellets);
      }

      const std::vector<int> &observation_shape() const { return observations[0].shape(); }

      /**
       * Returns the current state of the world without advancing through time
       * @return An Observation object containing all of the
       * locations of every entity in the current state of the game world
       */
      const std::vector<Observation> &get_observations() const { return observations; }

      /* since we reuse the observation's data buffer for each step,
       * we need to have the data cleared at the beginning of each step */
      void _step_hook() override {
        for (auto &observation : observations)
          observation.clear_data();
      }

      /* allows for intermediate grid frames to be stored in the GridObservation */
      void _partial_observation(int agent_index, int tick_index) override {
        assert(agent_index < this->num_agents());
        assert(tick_index < this->ticks_per_step());

        auto &player = this->engine_.player(this->pids_[agent_index]);
        if (player.dead()) return;

        Observation &observation = observations[agent_index];

        auto &state = this->engine_.game_state();
        int frame_index = tick_index - (this->ticks_per_step() - observation.num_frames());
        if (frame_index >= 0)
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
      std::vector<GridObservation> observations;

#ifdef RENDERABLE
      std::unique_ptr<agario::Renderer> renderer;
      std::shared_ptr<Window> window;
#endif
    };

  } // namespace env
} // namespace agario
