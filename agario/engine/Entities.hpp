#ifndef AGARIO_ENTITIES_HPP
#define AGARIO_ENTITIES_HPP

#include "types.hpp"
#include "Ball.hpp"
#include "settings.hpp"

namespace Agario {

  class Pellet : public Ball {
  public:
    using Ball::Ball;
    length radius() const override { return PELLET_SIZE; }
    Agario::mass mass () const override { return PELLET_MASS; }
    ~Pellet() override {}
  private:
  };

  class Food : public MovingBall {
  public:
    using MovingBall::MovingBall;
    length radius() const override { return FOOD_SIZE; }
    Agario::mass mass() const override { return FOOD_MASS; }
  private:
  };

  class Virus : public MovingBall {
  public:
    using MovingBall::MovingBall;

    // todo: viruses have variable mass and size
    length radius() const override { return VIRUS_SIZE; }
    Agario::mass mass() const override { return VIRUS_MASS; }
  private:
  };
}

#endif //AGARIO_ENTITIES_HPP
