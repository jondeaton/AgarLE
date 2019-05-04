#pragma once

#include "core/Ball.hpp"
#include "core/types.hpp"
#include "core/utils.hpp"
#include "core/settings.hpp"
#include "core/renderables.hpp"

#define PELLET_MASS 1
#define FOOD_MASS 10
#define VIRUS_MASS 100

#define PELLET_SIDES 5
#define VIRUS_SIDES 150
#define FOOD_SIDES 7
#define CELL_SIDES 50

namespace agario {

  template<bool renderable>
  class Pellet : public std::conditional<renderable, RenderableBall<PELLET_SIDES>, Ball>::type {
  public:
    typedef typename std::conditional<renderable, RenderableBall<PELLET_SIDES>, Ball>::type Super;

    // constructors explicitly put here because of virtual inheritance rules...
    Pellet(distance x, distance y) : Ball(x, y), Super(x, y) {}
    template<typename Loc, bool r = renderable>
    explicit Pellet(Loc &&loc, typename std::enable_if<r>::type* = 0) : Ball(loc), Super(loc) {}
    template<typename Loc, bool r = renderable>
    explicit Pellet(Loc &&loc, typename std::enable_if<!r>::type* = 0) : Ball(loc) {}

    distance radius() const override { return radius_conversion(mass()); }

    agario::mass mass() const override { return PELLET_MASS; }

  private:
  };


  template<bool renderable>
  class Food : public std::conditional<renderable, RenderableMovingBall<FOOD_SIDES>, MovingBall>::type {
  public:

    // inherit constructors
    typedef typename std::conditional<renderable, RenderableMovingBall<FOOD_SIDES>, MovingBall>::type Super;
    using Super::Super;

    Food(distance x, distance y) : Ball(x, y), Super(x, y) {}

    Food(Location &&loc, Velocity &vel) : Ball(loc), Super(loc, vel) {}

    Food(Location &loc, Velocity &vel) : Ball(loc), Super(loc, vel) {}

    distance radius() const override { return radius_conversion(mass()); }

    agario::mass mass() const override { return FOOD_MASS; }

  private:
  };


  template<bool>
  struct oVirus : virtual public RenderableMovingBall<VIRUS_SIDES> {
    void _create_vertices() override {
      auto num_verts = RenderableMovingBall<VIRUS_SIDES>::NVertices;
      this->circle.verts[0] = 0;
      this->circle.verts[1] = 0;
      this->circle.verts[2] = 0;
      for (unsigned i = 1; i < num_verts; i++) {
        auto radius = 1 + sin(30 * M_PI * i / VIRUS_SIDES) / 15;
        this->circle.verts[i * 3] = radius * cos(i * 2 * M_PI / VIRUS_SIDES);
        this->circle.verts[i * 3 + 1] = radius * sin(i * 2 * M_PI / VIRUS_SIDES);
        this->circle.verts[i * 3 + 2] = 0;
      }
    }
  };

  template<>
  struct oVirus<false> {
  };

  template<bool renderable>
  class Virus : public std::conditional<renderable, oVirus<renderable>, MovingBall>::type {
  public:

    // inherit constructors
    typedef typename std::conditional<renderable, RenderableMovingBall<VIRUS_SIDES>, MovingBall>::type Super;


    template <typename Loc, typename Vel>
    Virus(Loc &&loc, Vel &&vel) : Ball(loc), Super(loc, vel) {
      if constexpr (renderable) this->color = agario::color::green;
    }

    template <typename Loc>
    explicit Virus(Loc &&loc) : Virus(loc, Velocity()) {}
    Virus(distance x, distance y) : Virus(Location(x, y)) {}

    distance radius() const override { return radius_conversion(mass()); }

    agario::mass mass() const override { return VIRUS_MASS; }

  private:
  };

  template<bool renderable>
  class Cell : public std::conditional<renderable, RenderableMovingBall<CELL_SIDES>, MovingBall>::type {
  public:
    typedef typename std::conditional<renderable, RenderableMovingBall<CELL_SIDES>, MovingBall>::type Super;
    using Super::Super;

    // gotta redeclare all the constructors because of virtual inheritance...
    template<typename Loc, typename Vel>
    Cell(Loc &&loc, Vel &&vel, agario::mass mass) : Ball(loc), Super(loc, vel),
                                                    _mass(mass), _can_recombine(false) {
      set_mass(mass);
      _recombine_timer = std::chrono::steady_clock::now();
    }

    template<typename Loc>
    Cell(Loc &&loc, agario::mass mass) : Cell(loc, Velocity(), mass) {}

    Cell(distance x, distance y, agario::mass mass) : Cell(Location(x, y), Velocity(), mass) {}

    agario::mass mass() const override { return _mass; }

    distance radius() const override {
      return radius_conversion(mass());
    }

    void move(float dt) override {
      this->x += (this->velocity.dx + splitting_velocity.dx) * dt;
      this->y += (this->velocity.dy + splitting_velocity.dy) * dt;
    }

    void set_mass(agario::mass new_mass) {
      _mass = std::max<agario::mass>(new_mass, CELL_MIN_SIZE);
    }

    void increment_mass(agario::mass inc) { set_mass(mass() + inc); }

    void reduce_mass_by_factor(float factor) { set_mass(mass() / factor); }

    bool can_recombine() {
      if (_can_recombine) return true;
      _can_recombine = std::chrono::steady_clock::now() >= _recombine_timer;
      return _can_recombine;
    }

    void reset_recombine_timer() {
      _recombine_timer = std::chrono::steady_clock::now() +
                         std::chrono::seconds(RECOMBINE_TIMER_SEC);
      _can_recombine = false;
    }

    agario::Velocity splitting_velocity;
    agario::time _recombine_timer;

  private:
    agario::mass _mass;
    bool _can_recombine;
  };

}
