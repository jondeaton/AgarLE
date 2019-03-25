#pragma once

#include "Ball.hpp"
#include "types.hpp"
#include "settings.hpp"

namespace Agario {

  class Pellet : public Ball {
  public:
    using Ball::Ball;
    distance radius() const override { return PELLET_SIZE; }
    Agario::mass mass () const override { return PELLET_MASS; }
  private:
  };

  class Food : public MovingBall {
  public:
    using MovingBall::MovingBall;
    distance radius() const override { return FOOD_SIZE; }
    Agario::mass mass() const override { return FOOD_MASS; }
  private:
  };

  class Virus : public MovingBall {
  public:
    using MovingBall::MovingBall;

    // todo: viruses have variable mass and size
    distance radius() const override { return VIRUS_SIZE; }
    Agario::mass mass() const override { return VIRUS_MASS; }
  private:
  };
}
