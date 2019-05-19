#pragma once

#include <engine/Engine.hpp>
#include <core/types.hpp>
#include <core/Entities.hpp>
#include <core/Ball.hpp>
#include <bots/bots.hpp>

#include "rendering/types.hpp"
#include "rendering/FrameBufferObject.hpp"
#include "rendering/renderer.hpp"
#include "envs/BaseEnvironment.hpp"

#define PIXEL_LEN 3

namespace agario {
  namespace env {


    class ScreenObservation {
    public:
      explicit ScreenObservation(int num_frames, screen_len width, screen_len height) :
        _num_frames(num_frames), _width(width), _height(height) {
        _frame_data = new std::uint8_t[size()];
      }

      const std::uint8_t *frame_data() const { return _frame_data; }

      std::size_t size() const {
        return _num_frames * _width * _height * PIXEL_LEN;
      }

      void clear() {
        std::fill(_frame_data, _frame_data + size(), 0);
      }

      std::uint8_t *frame_data(int frame_index) {
        if (frame_index >= _num_frames)
          throw FBOException("Frame index " + std::to_string(frame_index) + " out of bounds");

        auto data_index = frame_index * _width * _height * PIXEL_LEN;
        return &_frame_data[data_index];
      }

      int num_frames() const { return _num_frames; }

      std::vector<int> shape() const {
        return {_num_frames, _width, _height, PIXEL_LEN};
      }

      std::vector<ssize_t> strides() const {
        return {
          _width * _height * PIXEL_LEN * dtype_size,
                   _height * PIXEL_LEN * dtype_size,
                             PIXEL_LEN * dtype_size,
                                         dtype_size
        };
      }

      ~ScreenObservation() { delete[] _frame_data; }
    private:
      int _num_frames;
      const int _width;
      const int _height;
      static constexpr ssize_t dtype_size = sizeof(std::uint8_t);
      std::uint8_t *_frame_data;
    };

    template<bool renderable>
    class ScreenEnvironment : public BaseEnvironment<renderable> {
      typedef agario::Player<renderable> Player;

    public:
      typedef BaseEnvironment<renderable> Super;

      explicit ScreenEnvironment(int frames_per_step, int arena_size, bool pellet_regen,
                                 int num_pellets, int num_viruses, int num_bots,
                                 screen_len screen_width, screen_len screen_height) :
        Super(frames_per_step, arena_size, pellet_regen, num_pellets, num_viruses, num_bots),
        _observation(frames_per_step, screen_width, screen_height),
        frame_buffer(std::make_shared<FrameBufferObject>(screen_width, screen_height)),
        renderer(frame_buffer, this->engine.arena_width(), this->engine.arena_height()) {}

      const ScreenObservation &get_state() const { return _observation; }
      screen_len screen_width() const { return frame_buffer->width(); }
      screen_len screen_height() const { return frame_buffer->height(); }

    private:

      ScreenObservation _observation;
      std::shared_ptr<FrameBufferObject> frame_buffer;
      agario::Renderer renderer;

      // stores current frame into buffer containing the next observation
      void _partial_observation(Player &player, int frame_index) override {
        renderer.render_screen(player, this->engine.game_state());
        void *data = _observation.frame_data(frame_index);
        frame_buffer->copy(data);
      }

    };

  } // namespace env
} // namespace agario




