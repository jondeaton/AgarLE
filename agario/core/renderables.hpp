#pragma once

#include <core/Ball.hpp>

#define CIRCLE_SIDES 50
#define CIRCLE_VERTS (CIRCLE_SIDES + 2)
#define COLOR_LEN 3

namespace Agario {

  struct Circle {
    float verts[3 * CIRCLE_VERTS];
    float color[COLOR_LEN];
    unsigned int vao; // vertex attribute object
    unsigned int vbo; // vertex buffer object (gpu memory)
  };

  class RenderableBall : public Ball {
    using Ball::Ball;
    Circle circle;
  };

  class RenderableMovingBall : public MovingBall {
    using MovingBall::MovingBall;
    Circle circle;
  };

}