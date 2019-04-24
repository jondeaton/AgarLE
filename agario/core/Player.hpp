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

namespace agario {

  template<bool renderable>
  class Player {

  public:

    typedef Cell <renderable> Cell;

    Player() = delete;

    template <typename Loc>
    Player(agario::pid pid, std::string name, Loc&& loc, agario::color color) :
      action(none), target(0, 0),
      split_cooldown(0), feed_cooldown(0),
      _pid(pid), _name(std::move(name)), _score(0),
      _color(color) {
      add_cell(loc, CELL_MIN_SIZE);
    }

    Player(agario::pid pid, std::string name, agario::color color) :
      Player(pid, name, Location(0, 0), color) { }

    Player(agario::pid pid, std::string name) :
      Player(pid, name, agario::color::blue) {}

    std::vector<Cell> cells;
    agario::action action;
    Location target;
    agario::tick split_cooldown;
    agario::tick feed_cooldown;

    agario::color color() const {
      return _color;
    }

    template<typename... Args>
    void add_cell(Args &&... args) {
      cells.emplace_back(std::forward<Args>(args)...);
      if constexpr (renderable)
        cells.back().color = _color;
    }
    
    bool dead() const { return cells.size() == 0; }

    void set_score(score new_score) { _score = new_score; }

    void increment_score(score inc) { _score += inc; }

    agario::distance x() const {
      agario::distance x_ = 0;
      for (auto &cell : cells)
        x_ += cell.x * cell.mass();
      return x_ / mass();
    }

    agario::distance y() const {
      agario::distance y_ = 0;
      for (auto &cell : cells)
        y_ += cell.y * cell.mass();
      return y_ / mass();
    }

    agario::Location location() const {
      return agario::Location(x(), y());
    }

    // Total mass of the player (sum of masses of all cells)
    agario::mass mass() const {
      agario::mass total_mass = 0;
      for (auto &cell : cells)
        total_mass += cell.mass();
      return total_mass;
    }

    agario::score score() const { return mass(); }

    bool operator<(const Player &other) const {
      return score() < other.score();
    }

    agario::pid pid() const { return _pid; }

    std::string name() const { return _name; }

    bool operator==(const Player &other) {
      return this->_pid == other.pid();
    }

    typename std::enable_if<renderable, void>::type
    draw(Shader &shader) {
      for (auto &cell : cells)
        cell.draw(shader);
    }

  private:
    agario::pid _pid;
    std::string _name;
    agario::score _score;
    agario::color _color;
  };

}
