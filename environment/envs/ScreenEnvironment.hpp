#pragma once

#include <engine/Engine.hpp>
#include <core/types.hpp>
#include <core/Entities.hpp>
#include <core/Ball.hpp>
#include <bots/bots.hpp>

#include "rendering/FrameBufferObject.hpp"
#include "rendering/renderer.hpp"

#define PIXEL_SIZE 3
#define DEFAULT_DT (1.0 / 60)

namespace agario {
  namespace env {
    namespace screen {

      typedef double reward;

      template<unsigned W, unsigned H>
      class Observation {
      public:
        explicit Observation(unsigned num_frames) : _num_frames(num_frames) {
          _frame_data = new std::uint8_t[size()];
        }

        const std::uint8_t *frame_data() const { return _frame_data; }

        std::size_t size() const {
          return _num_frames * W * H * PIXEL_SIZE;
        }

        void clear() { std::fill(_frame_data, _frame_data + size(), 0); }

        std::uint8_t *frame_data(unsigned frame_index) {
          if (frame_index >= _num_frames)
            throw FBOException("Frame index " + std::to_string(frame_index) + " out of bounds");

          return &_frame_data[frame_index * W * H * PIXEL_SIZE];
        }

        unsigned num_frames() const { return _num_frames; }

        ~Observation() { delete[] _frame_data; }
      private:
        unsigned _num_frames;
        std::uint8_t *_frame_data;
      };

      template<bool renderable, unsigned W, unsigned H>
      class Environment {
        typedef agario::Player<renderable> Player;
        typedef agario::bot::HungryBot<renderable> HungryBot;
        typedef agario::bot::HungryShyBot<renderable> HungryShyBot;

        typedef Observation<W, H> Observation;

      public:

        explicit Environment(int frames_per_step) :
          engine(), _observation(frames_per_step),
          _num_frames(frames_per_step), _done(false), step_dt(DEFAULT_DT),
          frame_buffer(std::make_shared<FrameBufferObject<W, H>>()),
          renderer(frame_buffer, engine.arena_width(), engine.arena_height()) {
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
            _store_observation(player, i);
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

        int _num_frames;
        bool _done;
        std::chrono::duration<float> step_dt;

        Observation _observation;
        std::shared_ptr<FrameBufferObject<W, H>> frame_buffer;
        agario::Renderer renderer;

        void add_bots() {
          for (int i = 0; i < 10; i++)
            engine.template add_player<HungryBot>("hungry");

          for (int i = 0; i < 25; i++)
            engine.template add_player<HungryShyBot>("shy");
        }

        void _store_observation(Player &player, unsigned frame_index) {
          renderer.render_screen(player, engine.game_state());
          void *data = _observation.frame_data(frame_index);
          frame_buffer->copy(data);
        }

      };
    }
  }
}




