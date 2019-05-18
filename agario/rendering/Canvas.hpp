#pragma once

#include "rendering/types.hpp"

class Canvas {
public:
  virtual screen_len width() const = 0;
  virtual screen_len height() const = 0;
  virtual float aspect_ratio() const { return (float) width() / height(); }
  virtual ~Canvas() = default;
};