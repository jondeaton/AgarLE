#pragma once

#include <core/Ball.hpp>
#include <core/types.hpp>
#include <core/utils.hpp>

#include <core/renderables.hpp>

#define PELLET_SIZE 1
#define PELLET_MASS 1

#define FOOD_SIZE 10
#define FOOD_MASS 10
#define FOOD_SPEED 10

#define VIRUS_SIZE 75
#define VIRUS_MASS 0

namespace Agario {

//  template <bool renderable>
//  class Pellet : public std::conditional<renderable, RenderableBall, Ball>::type {
  class Pellet : public Ball {
public:

    Agario::distance radius() const override { return PELLET_SIZE; }
    Agario::mass mass () const override { return PELLET_MASS; }
  private:
  };


  template <bool renderable>
  class Food : public std::conditional<renderable, RenderableMovingBall, MovingBall>::type {
  public:
    using MovingBall::MovingBall;
    Agario::distance radius() const override { return FOOD_SIZE; }
    Agario::mass mass() const override { return FOOD_MASS; }
  private:
  };


  template <bool renderable>
  class Virus : public std::conditional<renderable, RenderableMovingBall, MovingBall>::type {
  public:
    using MovingBall::MovingBall;
    // todo: viruses have variable mass and size
    Agario::distance radius() const override { return VIRUS_SIZE; }
    Agario::mass mass() const override { return VIRUS_MASS; }
  private:
  };

  template <bool renderable>
  class Cell : public std::conditional<renderable, RenderableMovingBall, MovingBall>::type {
  public:
    typedef typename std::conditional<renderable, RenderableMovingBall, MovingBall>::type Super;

    explicit Cell(distance x, distance y, Agario::mass mass) :
      Super(x, y) { set_mass(mass); }

    explicit Cell(Location &&loc, Velocity &vel, Agario::mass mass) :
      Super(loc, vel), _mass(mass) { }

    explicit Cell(Location &loc, Velocity &vel, Agario::mass mass) :
      Super(loc, vel), _mass(mass) { }

    Agario::mass mass() const override { return _mass; }
    distance radius() const override {
      return radius_conversion(mass());
    }

    void set_mass(Agario::mass new_mass) {
      _mass = std::max<Agario::mass>(new_mass, CELL_MIN_SIZE);
    }

    void increment_mass(int inc) { set_mass(mass() + inc); }
    void reduce_mass_by_factor(float factor) { set_mass(mass() / factor); }

  private:
    Agario::mass _mass;
  };

}
