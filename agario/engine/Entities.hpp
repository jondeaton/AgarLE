#ifndef AGARIO_ENTITIES_HPP
#define AGARIO_ENTITIES_HPP

#include "types.hpp"
#include "Ball.hpp"
#include "settings.hpp"

namespace Agario {

  class Pellet : public Ball {
  public:
    explicit Pellet(position x, position y) : Ball(x, y) { }
    explicit Pellet(Location &loc) : Ball(loc) { }
    length radius() const override { return FOOD_SIZE; }
    Agario::mass mass () const override { return FOOD_MASS; }
  private:
  };

  class MovingBall : public Ball {
  public:
    explicit MovingBall(position x, position y) : Ball(x, y) { }
    explicit MovingBall(Location &loc) : Ball(loc) { }
    explicit MovingBall(Location &loc, Velocity &v) : Ball(loc), velocity(v) { }
    Velocity velocity;
  };

  class Food : public MovingBall {
  public:
    explicit Food(position x, position y) : MovingBall(x, y) { }
    explicit Food(Location &loc) : MovingBall(loc) { }
    length radius() const override { return MASS_SIZE; }
    Agario::mass mass() const override { return MASS_MASS; }
  private:
  };

  class Virus : public MovingBall {
  public:
    explicit Virus(position x, position y) : MovingBall(x, y) { }
    explicit Virus(Location &loc) : MovingBall(loc) { }

    // todo: viruses have variable mass and size
    length radius() const override { return VIRUS_SIZE; }
    Agario::mass mass() const override { return VIRUS_MASS; }
  private:
  };
}

#endif //AGARIO_ENTITIES_HPP
