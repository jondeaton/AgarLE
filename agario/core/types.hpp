#pragma once
#include <cmath>

template<class T, int type_distinguisher>
class numWrapper {
  // adapted from https://stackoverflow.com/questions/17793298/c-class-wrapper-around-fundamental-types
  T value;
public:
  typedef T value_type;
  numWrapper() : value() {}
  numWrapper(T v) : value(v) {}
  operator T() const {return value;}

  //modifiers
  numWrapper& operator=(T v) {value = v; return *this;}
  numWrapper& operator+=(T v) {value += v; return *this;}
  numWrapper& operator-=(T v) {value -= v; return *this;}
  numWrapper& operator*=(T v) {value *= v; return *this;}
  numWrapper& operator/=(T v) {value /= v; return *this;}
  numWrapper& operator%=(T v) {value %= v; return *this;}
  numWrapper& operator++() {++value; return *this;}
  numWrapper& operator--() {--value; return *this;}
  numWrapper operator++(int) {return numWrapper(value++);}
  numWrapper operator--(int) {return numWrapper(value--);}
  numWrapper& operator&=(T v) {value&=v; return *this;}
  numWrapper& operator|=(T v) {value|=v; return *this;}
  numWrapper& operator^=(T v) {value^=v; return *this;}
  numWrapper& operator<<=(T v) {value<<=v; return *this;}
  numWrapper& operator>>=(T v) {value>>=v; return *this;}

  //accessors
  numWrapper operator+() const {return numWrapper(+value);}
  numWrapper operator-() const {return numWrapper(-value);}
  numWrapper operator!() const {return numWrapper(!value);}
  numWrapper operator~() const {return numWrapper(~value);}

  //friends
  friend numWrapper operator+(numWrapper iw, numWrapper v) {return iw+=v;}
  template <typename U>
  friend numWrapper operator+(numWrapper iw, U v) {return iw+=v;}
  template <typename U>
  friend numWrapper operator+(U v, numWrapper iw) {return numWrapper(v)+=iw;}

  friend numWrapper operator-(numWrapper iw, numWrapper v) {return iw-=v;}
  template <typename U>
  friend numWrapper operator-(numWrapper iw, U v) {return iw-=v;}
  template <typename U>
  friend numWrapper operator-(U v, numWrapper iw) {return numWrapper(v)-=iw;}

  friend numWrapper operator*(numWrapper iw, numWrapper v) {return iw*=v;}
  template <typename U>
  friend numWrapper operator*(numWrapper iw, U v) {return iw*=v;}
  template <typename U>
  friend numWrapper operator*(U v, numWrapper iw) {return numWrapper(v)*=iw;}

  friend numWrapper operator/(numWrapper iw, numWrapper v) {return iw/=v;}
  template <typename U>
  friend numWrapper operator/(numWrapper iw, U v) {return iw/=v;}
  template <typename U>
  friend numWrapper operator/(U v, numWrapper iw) {return numWrapper(v)/=iw;}

  // overload the other operators (%, &, |, ^, <<, >>) in the same way if needed
};

namespace agario {
  enum _type_id { _distance, _angle };
  using distance = numWrapper<float, _distance>;
  using angle = numWrapper<float, _angle>;

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

  template <typename T>
  inline Coordinate<T> operator/(Coordinate<T> v, distance norm) {
    v.x /= norm;
    v.y /= norm;
    return v;
  }

  template <typename T>
  inline Coordinate<T> operator*(Coordinate<T> v, distance norm) {
    v.x *= norm;
    v.y *= norm;
    return v;
  }

  typedef Coordinate<agario::distance> Location;


  class Velocity {
  public:
    explicit Velocity() : dx(0), dy(0) { }
    explicit Velocity(agario::Location dir) : dx(dir.x), dy(dir.y) {}
    explicit Velocity(agario::distance dx, agario::distance dy) : dx(dx), dy(dy) { }
    explicit Velocity(agario::angle angle, agario::distance speed) :
      dx(speed * std::cos(angle)), dy(speed * std::sin(angle)) { }

    agario::angle direction() const {
      auto angle = std::atan(dx / dy);
      if (dx < 0) {
        if (dy > 0) angle += M_PI;
        else angle -= M_PI;
      }
      return static_cast<agario::angle>(angle);
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
