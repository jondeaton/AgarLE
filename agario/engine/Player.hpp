#ifndef AGARIO_PLAYER_HPP
#define AGARIO_PLAYER_HPP

#include <string>

#include "types.hpp"
#include "Ball.hpp"
#include "Engine.hpp"
#include "utils.hpp"


namespace Agario {

  class Player;

  class Cell : public Ball {
  public:
    explicit Cell(position x, position y, mass mass,
                  Player &player) : Ball(x, y, radius_conversion(mass)),
                                    player(player), mass(mass) { }

  private:
    Player &player;
    mass mass;
  };


  class Player {
    friend class Engine;

  public:
    explicit Player(pid pid, std::string name) :
      pid(pid),
      name(std::move(name)) , score(0) { }

    void add_cell(position x, position y, length radius) {
      cells.emplace_back(x, y, radius, *this);
    }

    void set_score(score new_score) { score = new_score; }
    void increment_score(score inc) { score += inc; }

  private:
    const pid pid;
    std::string name;
    std::vector<Cell> cells;
    score score;
  };

}

#endif //AGARIO_PLAYER_HPP
