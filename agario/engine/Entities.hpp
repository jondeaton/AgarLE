#ifndef AGARIO_ENTITIES_HPP
#define AGARIO_ENTITIES_HPP

#include "types.hpp"
#include "Ball.hpp"

#define FOOD_SIZE 1
#define MASS_SIZE 5
#define VIRUS_SIZE 75

namespace Agario {

  class Food : public Ball {
  public:
    explicit Food(position x, position y) :
      Ball(x, y, FOOD_SIZE) { }
  private:
  };

  class Mass : public Ball {
  public:
    explicit Mass(position x, position y) : Ball(x, y, MASS_SIZE) { }
  private:
  };

  class Virus : public Ball {
  public:
    explicit Virus(position x, position y) : Ball(x, y, VIRUS_SIZE) { }
  private:
  };

}

#endif //AGARIO_ENTITIES_HPP
