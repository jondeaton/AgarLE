#ifndef AGARIO_PLAYER_HPP
#define AGARIO_PLAYER_HPP

#include <string>
#include <algorithm>

#include "types.hpp"
#include "Entities.hpp"
#include "Ball.hpp"
#include "Engine.hpp"
#include "utils.hpp"


namespace Agario {

  class Player;

  class Cell : public MovingBall {
    friend class Engine; // todo: remove?
  public:
    explicit Cell(position x, position y, Agario::mass mass, Player &player) :
      MovingBall(x, y), _player(player) { set_mass(mass); }

    explicit Cell(Location &loc, Velocity &vel, Agario::mass mass, Player &player) :
      MovingBall(loc, vel), _player(player), _mass(mass) { }

    Agario::mass mass() const override { return _mass; }
    length radius() const override {
      return radius_conversion(mass());
    }

    void set_mass(Agario::mass new_mass) {
      _mass = std::max<Agario::mass>(new_mass, CELL_MIN_SIZE);
    }

    void increment_mass(int inc) { set_mass(mass() + inc); }
    void reduce_mass_by_factor(float factor) { set_mass(mass() / factor); }

    Player &player() const { return _player; }

  private:
    Player &_player;
    Agario::mass _mass;
    // todo: cache radius better performance?
  };

  enum action { none = 0, feed = 1, split = 2 };

  class Player {
    friend class Engine;

  public:
    explicit Player(pid pid, std::string name) :
      pid(pid),
      name(std::move(name)), action(none), target(0, 0), _score(0) { }

    void add_cell(position x, position y, length radius) {
      cells.emplace_back(x, y, radius, *this);
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

  private:
    const pid pid;
    std::string name;
    std::vector<Cell> cells;

    Agario::action action;
    bool has_target = false;
    Location target;

    Agario::score _score;
  };

}

#endif //AGARIO_PLAYER_HPP
