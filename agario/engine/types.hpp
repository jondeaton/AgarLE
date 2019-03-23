#pragma once
#include <cmath>

namespace Agario {

  typedef double distance;
  typedef float angle;

  typedef unsigned int mass;
  typedef unsigned int score;

  typedef unsigned short pid;
  typedef unsigned long tick;

  enum action { none = 0, feed = 1, split = 2 };
  enum Color { red = 0, orange = 1, yellow = 2, green = 3, blue = 4, purple = 5 };

  template<typename T>
  class Coordinate {
  public:
    explicit Coordinate(T x, T y) : x(x), y(y) { }
    T x;
    T y;

    Coordinate& operator +=(const Coordinate &rhs) {
      x += rhs.x;
      y += rhs.y;
      return *this;
    }

    Coordinate& operator -=(const Coordinate &rhs) {
      x -= rhs.x;
      y -= rhs.y;
      return *this;
    }

    Agario::distance norm_sqr() const {
      auto dx = std::abs(x);
      auto dy = std::abs(y);
      return dx * dx + dy * dy;
    }

    Agario::distance norm() const {
      return std::sqrt(norm_sqr());
    }

    void normalize() {
      *this = *this / this->norm();
    }

    Coordinate normed() {
      return *this / this->norm();
    }
  };

  template <typename T>
  inline Coordinate<T> operator+(Coordinate<T> lhs, const Coordinate<T>& rhs) {
    lhs += rhs;
    return lhs;
  }

  template <typename T>
  inline Coordinate<T> operator-(Coordinate<T> lhs, const Coordinate<T>& rhs) {
    lhs -= rhs;
    return lhs;
  }

  template <typename T, typename U>
  inline Coordinate<T> operator/(Coordinate<T> v, U norm) {
    v.x /= norm;
    v.y /= norm;
    return v;
  }

  template <typename T, typename U>
  inline Coordinate<T> operator*(Coordinate<T> v, U norm) {
    v.x *= norm;
    v.y *= norm;
    return v;
  }

  typedef Coordinate<Agario::distance> Location;



  class Velocity {
  public:
    explicit Velocity() : dx(0), dy(0) { }
    explicit Velocity(Agario::Location dir) : dx(dir.x), dy(dir.y) {}
    explicit Velocity(Agario::distance dx, Agario::distance dy) : dx(dx), dy(dy) { }
    explicit Velocity(Agario::angle angle, Agario::distance speed) :
      dx(speed * std::cos(angle)), dy(speed * std::sin(angle)) { }

    Agario::angle direction() const {
      auto angle = std::atan(dx / dy);
      if (dx < 0) {
        if (dy > 0) angle += M_PI;
        else angle -= M_PI;
      }
      return static_cast<Agario::angle>(angle);
    }

    Velocity& operator +=(const Velocity &rhs) {
      dx += rhs.dx;
      dy += rhs.dy;
      return *this;
    }

    Velocity& operator -=(const Velocity &rhs) {
      dx -= rhs.dx;
      dy -= rhs.dy;
      return *this;
    }

    distance dx;
    distance dy;
  };

  inline Velocity operator+(Velocity lhs, const Velocity& rhs) {
    lhs += rhs;
    return lhs;
  }

  inline Velocity operator-(Velocity lhs, const Velocity& rhs) {
    lhs -= rhs;
    return lhs;
  }

}
