#pragma once

class Canvas {
public:
  virtual int width() const = 0;
  virtual int height() const = 0;
  virtual float aspect_ratio() const { return (float) width() / height(); }
  virtual ~Canvas() = default;
};