#ifndef AGARIO_BALL_HPP
#define AGARIO_BALL_HPP

#include "types.hpp"
#include <math.h>

namespace Agario {

  class Ball {
  public:

    explicit Ball(position x, position y, length radius) :
      x(x), y(y), r(radius) { }

    length height() const { return 2 * r; }
    length width() const { return 2 * r; }
    length raidus() const { return r; }

    void set_radius(length radius) { r = radius; }

    bool collides_with(const Ball &other) {
      auto sqr_rads = pow(r + other.raidus(), 2);
      return sqr_rads <= sqr_distance_to(other);
    }

    position x;
    position y;
    length r;

  private:

    length sqr_distance_to(const Ball &other) {
      auto dx = std::abs(x - other.x);
      auto dy = std::abs(y - other.y);
      return dx * dx + dy * dy;
    }

  };

}

#endif //AGARIO_BALL_HPP
