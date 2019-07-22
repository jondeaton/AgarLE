#pragma once

#include <agario/engine/Engine.hpp>
#include <agario/core/types.hpp>
#include <agario/core/Entities.hpp>
#include <agario/core/Ball.hpp>
#include <agario/bots/bots.hpp>
#include "agario/engine/GameState.hpp"

#include "environment/envs/BaseEnvironment.hpp"

#ifdef RENDERABLE

#include "core/renderables.hpp"
#include "rendering/window.hpp"
#include "rendering/renderer.hpp"

#endif

namespace agario {
  namespace env {

    template<bool renderable>
    class FullObservation {
      typedef GameState<renderable> GameState;
      typedef Player<renderable> Player;
      typedef Cell<renderable> Cell;
      typedef Pellet<renderable> Pellet;
      typedef Virus<renderable> Virus;
      typedef Food<renderable> Food;

    public:
      explicit FullObservation(const Player &player, const GameState &game_state) {
        _store_entities<Pellet, 2>(game_state.pellets);
        _store_entities<Virus,  2>(game_state.viruses);
        _store_entities<Food,   2>(game_state.foods);
        _store_entities<Cell,   5>(player.cells);
        for (auto &pair : game_state.players) {
          auto &other_player = *pair.second;
          _store_entities<Cell, 5>(other_player.cells);
        }
      }

      const std::vector<float *> &data() const { return _data; }
      const std::vector<std::vector<int>> shapes() const { return _shapes; }

      ~FullObservation() {
        for (float *d : _data)
          delete[] d;
      }

    private:
      std::vector<float *> _data;
      std::vector<std::vector<int>> _shapes;

      template<typename T, int NumAttr>
      void _store_entities(const std::vector<T> &entities) {
        int num = entities.size();
        _data.push_back(new float[NumAttr * num]);
        int index = _data.size() - 1;
        _copy_entities(entities, _data[index]);
        _shapes.push_back({num, NumAttr});
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
    class FullEnvironment : public BaseEnvironment<renderable> {
      typedef agario::Player<renderable> Player;
      typedef FullObservation<renderable> FullObservation;

    public:
      typedef BaseEnvironment<renderable> Super;

      explicit FullEnvironment(int frames_per_step, int arena_size, bool pellet_regen,
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
      const FullObservation get_state() const {
        auto &player = this->engine.get_player(this->pid);
        return FullObservation(player, this->engine.get_game_state());
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
