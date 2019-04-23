#pragma once

#include <cmath>
#include <iostream>

#include "core/num_wrapper.hpp"

namespace agario {

  enum _type_id {
    _distance, _angle
  };
  using distance = numWrapper<float, _distance>;
  using angle = numWrapper<float, _angle>;

  typedef unsigned int mass;
  typedef unsigned int score;

  typedef unsigned short pid;
  typedef unsigned long tick;

  typedef std::chrono::steady_clock::time_point time;

  enum action {
    none = 0, feed = 1, split = 2
  };

  template<typename T>
  class Coordinate {
  public:
    explicit Coordinate(T x, T y) : x(x), y(y) {}

    T x;
    T y;

    Coordinate &operator+=(const Coordinate &rhs) {
      x += rhs.x;
      y += rhs.y;
      return *this;
    }

    Coordinate &operator-=(const Coordinate &rhs) {
      x -= rhs.x;
      y -= rhs.y;
      return *this;
    }

    agario::distance norm_sqr() const {
      auto dx = std::abs(x);
      auto dy = std::abs(y);
      return dx * dx + dy * dy;
    }

    agario::distance norm() const {
      return std::sqrt(norm_sqr());
    }

    void normalize() {
      *this = *this / this->norm();
    }

    Coordinate normed() {
      return *this / this->norm();
    }
  };

  template<typename T>
  std::ostream &operator<<(std::ostream &os, const Coordinate<T> &c) {
    return os << "(" << c.x << ", " << c.y << ")";
  }

  template<typename T>
  inline Coordinate<T> operator+(Coordinate<T> lhs, const Coordinate<T> &rhs) {
    lhs += rhs;
    return lhs;
  }

  template<typename T>
  inline Coordinate<T> operator-(Coordinate<T> lhs, const Coordinate<T> &rhs) {
    lhs -= rhs;
    return lhs;
  }

  template<typename T>
  inline Coordinate<T> operator/(Coordinate<T> v, distance norm) {
    v.x /= norm;
    v.y /= norm;
    return v;
  }

  template<typename T>
  inline Coordinate<T> operator*(Coordinate<T> v, distance norm) {
    v.x *= norm;
    v.y *= norm;
    return v;
  }

  typedef Coordinate<agario::distance> Location;


  class Velocity {
  public:
    explicit Velocity() : dx(0), dy(0) {}

    explicit Velocity(agario::Location dir) : dx(dir.x), dy(dir.y) {}

    explicit Velocity(agario::distance dx, agario::distance dy) : dx(dx), dy(dy) {}

    explicit Velocity(agario::angle angle, agario::distance speed) :
      dx(speed * std::cos(angle)), dy(speed * std::sin(angle)) {}

    void set_speed(float new_speed) {
      // sets the speed without changing direction
      dx *= new_speed / speed();
      dy *= new_speed / speed();
    }

    agario::angle direction() const {
      auto angle = std::atan(dx / dy);
      if (dx < 0) {
        if (dy > 0) angle += M_PI;
        else angle -= M_PI;
      }
      return static_cast<agario::angle>(angle);
    }

    void clamp_speed(float low, float high) {
      if (speed() > high)
        set_speed(high);
      else if (speed() < low)
        set_speed(low);
    }

    /**
     * modified the magnitude of the velocity without
     * changing it's direction by amount acc * dt
     * @param acc the acceleration
     * @param dt the time period over which time to
     * accelerate the velocity
     */
    void accelerate(float acc, float dt) {
      auto x_ratio = dx / magnitude();
      auto y_ratio = dy / magnitude();

      auto ddx = x_ratio * acc;
      dx += ddx * dt;

      auto ddy = y_ratio * acc;
      dy += ddy * dt;
    }

    /**
     * modifies the magnitude of the velocity by - decel * dt
     * letting the velocity come to zero if
     * @param decel deceleration magnitude
     * @param dt time period (seconds) over which to decelerate
     * the velocity
     */
    void decelerate(float decel, float dt) {
      auto x_ratio = dx / magnitude();
      auto y_ratio = dy / magnitude();

      auto ddx = x_ratio * decel;
      if (std::abs(ddx * dt) <= std::abs(dx))
        dx -= ddx * dt;
      else
        dx = 0;

      auto ddy = y_ratio * decel;
      if (std::abs(ddy * dt) <= std::abs(dy))
        dy -= ddy * dt;
      else
        dy = 0;
    }

    float magnitude() const {
      return sqrt(dx * dx + dy * dy);
    }

    float speed() const { return magnitude(); }

    Velocity &operator+=(const Velocity &rhs) {
      dx += rhs.dx;
      dy += rhs.dy;
      return *this;
    }

    Velocity &operator-=(const Velocity &rhs) {
      dx -= rhs.dx;
      dy -= rhs.dy;
      return *this;
    }

    template<typename T>
    Velocity &operator*=(const T &a) {
      this->dx *= a;
      this->dy *= a;
      return *this;
    }

    template<typename T>
    Velocity &operator/=(const T &a) {
      this->dx /= a;
      this->dy /= a;
      return *this;
    }

    distance dx;
    distance dy;
  };

  inline Velocity operator+(Velocity lhs, const Velocity &rhs) {
    lhs += rhs;
    return lhs;
  }

  inline Velocity operator-(Velocity lhs, const Velocity &rhs) {
    lhs -= rhs;
    return lhs;
  }

}
