#ifndef AGARIO_BALL_HPP
#define AGARIO_BALL_HPP

#include "types.hpp"
#include "Player.hpp"
#include <math.h>

namespace Agario {

  class Ball {
  public:
    explicit Ball(const Location &loc) : x(loc.x), y(loc.y) { }
    explicit Ball(Location &&loc) : x(loc.x), y(loc.y) { }
    explicit Ball(position x, position y) : Ball(Location(x, y)) { }

    virtual length radius() const = 0;
    virtual Agario::mass mass() const = 0;

    length height() const { return 2 * radius(); }
    length width() const { return 2 * radius(); }

    bool collides_with(const Ball &other) {
      auto sqr_rads = pow(radius() + other.radius(), 2);
      return sqr_rads <= sqr_distance_to(other);
    }

    Location location() const { return Location(x, y); }

    bool operator <(const Ball& other) const {
      return mass() * CELL_EAT_MARGIN < other.mass();
    }

    bool operator >(const Ball& other) const {
      return mass() > other.mass() * CELL_EAT_MARGIN;
    }

    bool operator ==(const Ball &other) const {
      return mass() == other.mass();
    }

    position x;
    position y;

  private:
    length sqr_distance_to(const Ball &other) {
      auto dx = std::abs(x - other.x);
      auto dy = std::abs(y - other.y);
      return dx * dx + dy * dy;
    }
  };

}

#endif //AGARIO_BALL_HPP
