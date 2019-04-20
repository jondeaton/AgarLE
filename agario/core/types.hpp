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
  std::ostream& operator<<(std::ostream& os, const Coordinate<T>& c) {
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

    agario::angle direction() const {
      auto angle = std::atan(dx / dy);
      if (dx < 0) {
        if (dy > 0) angle += M_PI;
        else angle -= M_PI;
      }
      return static_cast<agario::angle>(angle);
    }

    float speed() const {
      return sqrt(dx * dx + dy * dy);
    }

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

    template <typename T>
    Velocity &operator*=(const T& a) {
      this->dx *= a;
      this->dy *= a;
      return *this;
    }

    template <typename T>
    Velocity &operator/=(const T& a) {
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
