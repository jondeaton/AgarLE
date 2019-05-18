#pragma once

#include <engine/Engine.hpp>
#include <core/types.hpp>
#include <core/Entities.hpp>
#include <core/Ball.hpp>
#include <bots/bots.hpp>

#include "rendering/FrameBufferObject.hpp"
#include "rendering/renderer.hpp"
#include "envs/BaseEnvironment.hpp"

#define PIXEL_SIZE 3

namespace agario {
  namespace env {
    namespace screen {

      typedef double reward;

      class ScreenObservation {
      public:
        explicit ScreenObservation(unsigned num_frames, unsigned width, unsigned height) :
          _num_frames(num_frames), _width(width), _height(height) {
          _frame_data = new std::uint8_t[size()];
        }

        const std::uint8_t *frame_data() const { return _frame_data; }

        std::size_t size() const {
          return _num_frames * _width * _height * PIXEL_SIZE;
        }

        void clear() { std::fill(_frame_data, _frame_data + size(), 0); }

        std::uint8_t *frame_data(unsigned frame_index) {
          if (frame_index >= _num_frames)
            throw FBOException("Frame index " + std::to_string(frame_index) + " out of bounds");

          auto data_index = frame_index * _width * _height * PIXEL_SIZE;
          return &_frame_data[data_index];
        }

        unsigned num_frames() const { return _num_frames; }

        ~ScreenObservation() { delete[] _frame_data; }
      private:
        unsigned _num_frames;
        const unsigned _width;
        const unsigned _height;

        std::uint8_t *_frame_data;
      };

      template<bool renderable>
      class ScreenEnvironment : BaseEnvironment<renderable> {
        typedef agario::Player<renderable> Player;
        typedef agario::bot::HungryBot<renderable> HungryBot;
        typedef agario::bot::HungryShyBot<renderable> HungryShyBot;


      public:

        typedef BaseEnvironment<renderable> Super;

        explicit ScreenEnvironment(unsigned frames_per_step, unsigned arena_size, bool pellet_regen,
                                   unsigned num_pellets, unsigned num_viruses, unsigned num_bots) :
          Super(frames_per_step, arena_size, pellet_regen, num_pellets, num_viruses, num_bots),
          frame_buffer(nullptr),
          renderer(frame_buffer, this->engine.arena_width(), this->engine.arena_height()) {}


        void configure_observation(unsigned width, unsigned height) {
          std::make_shared<FrameBufferObject>(width, height);
        }

        const ScreenObservation &get_state() const { return _observation; }

      private:

        ScreenObservation _observation;
        std::shared_ptr<FrameBufferObject> frame_buffer;
        agario::Renderer renderer;

        // stores current frame into buffer containing the next observation
        void _partial_observation(Player &player, unsigned frame_index) override {
          renderer.render_screen(player, this->engine.game_state());
          void *data = _observation.frame_data(frame_index);
          frame_buffer->copy(data);
        }

      };
    }
  }
}




