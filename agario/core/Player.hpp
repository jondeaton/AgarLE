#pragma once

#include <string>
#include <algorithm>
#include <vector>

#include "core/Ball.hpp"
#include "core/types.hpp"
#include "core/Entities.hpp"
#include "utils.hpp"

#include <assert.h>
#include <type_traits>

namespace Agario {

  template <bool renderable>
  class Player {

  public:

    typedef Cell<renderable> Cell;

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

    // todo: cache this so that it doesn't have to be recomputed
    Agario::distance x() const {
      Agario::distance x = 0;
      for (auto &cell : cells)
        x += cell.x * cell.mass();
      return x / mass();
    }

    Agario::distance y() const {
      Agario::distance y = 0;
      for (auto &cell : cells)
        y += cell.y * cell.mass();
      return y / mass();
    }

    Agario::Location location() const {
      return Agario::Location(x(), y());
    }

    // Total mass of the player (sum of masses of all cells)
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


    template <typename=typename std::enable_if<renderable>::type>
      void draw(Shader &shader) const {
        for (auto &cell : cells)
          cell.draw(shader);
      }

  private:
    Agario::pid _pid;
    std::string _name;
    Agario::score _score;
  };

}
