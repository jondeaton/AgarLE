#pragma once

#include <string>
#include <algorithm>
#include <vector>
#include "types.hpp"
#include "Entities.hpp"
#include "Ball.hpp"
#include "utils.hpp"

#include <assert.h>
#include <type_traits>

namespace Agario {

  class Player;

  class Cell : public MovingBall {

  public:
    explicit Cell(distance x, distance y, Agario::mass mass) :
      MovingBall(x, y) { set_mass(mass); }

    explicit Cell(Location &&loc, Velocity &vel, Agario::mass mass) :
      MovingBall(loc, vel), _mass(mass) { }


    explicit Cell(Location &loc, Velocity &vel, Agario::mass mass) :
      MovingBall(loc, vel), _mass(mass) { }

//    length radius() const  {return 0;}
//    Agario::mass mass() const {return 0;}

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
    // const Agario::pid pid; // todo: maybe?
    Agario::mass _mass;
  };

  class Player {

  public:

    explicit Player(pid pid, std::string name) :
      action(none), target(0, 0),
      _pid(pid), _name(std::move(name)), _score(0) { }

    std::vector<Cell> cells;
    Agario::action action;
    Location target;

    template<typename... Args>
    void add_cell(Args&&... args) {
      cells.emplace_back(std::forward<Args>(args)...);
    }

    void set_score(score new_score) { _score = new_score; }
    void increment_score(score inc) { _score += inc; }

    /**
     * Total mass of the player (sum of masses of all cells)
     * @return
     */
    Agario::mass mass() const {
      Agario::mass total_mass = 0;
      for (auto &cell : cells)
        total_mass += cell.mass();
      return total_mass;
    }

    Agario::score score() const { return mass(); }

    bool operator <(const Player& other) const {
      return score() < other.score();
    }

    Agario::pid pid() const { return _pid; }
    std::string name() const { return _name; }

    bool operator==(const Player &other) {
      return this->_pid == other.pid();
    }

  private:
    Agario::pid _pid;
    std::string _name;
    Agario::score _score;
  };

}
