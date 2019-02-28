#include <iostream>
#include <algorithm>

#include "Engine.hpp"
#include "Entities.hpp"
#include "utils.hpp"

namespace Agario {

  void Engine::tick() {
    std::cout << "tick: " << ticks << std::endl;

    std::vector<Food> created_masses;
    for (Player &player : players)
      tick_player(player);

    ticks++;
  }

  std::vector<Player> &Engine::leaderboard() {
    std::sort(std::begin(players), std::end(players));
    return players;
  }

  void Engine::tick_player(Player &player) {
    move_player(player);

    std::vector<Cell> created_cells;
    for (Cell &cell : player.cells) {
      eat_pellets(cell);
      eat_food(cell);

      // todo: change VIRUS_SIZE to static member of Virus?
      check_virus_collisions(cell, created_cells);

      if (player.action == Agario::action::feed)
        emit_pellets(player);

      if (player.action == Agario::action::split) {
        // todo: split
      }
    }

    // add any cells that were created
    player.cells.insert(std::end(player.cells),
                        std::begin(created_cells),
                        std::end(created_cells));

    // player collisions
    for (Player &other : players) {
      (void) other;
      // todo: player collisions
    }

    // todo: recombine cells
    // todo: decrement recombine timers
  }

  void Engine::move_player(Player &player) {
    (void) player;
    // todo
  }

  void Engine::add_pellets(int num_pellets) {
    while (num_pellets > 0) {
      pellets.emplace_back(random_location());
      num_pellets--;
    }
  }

  void Engine::add_virus(int num_virus) {
    while (num_virus > 0) {
      viruses.emplace_back(random_location());
      num_virus--;
    }
  }

  void Engine::emit_pellets(Agario::Player &player) {

    // emit one pellet from each sufficiently large cell
    for (Cell &cell : player.cells) {

      // not big enough to emit pellet
      if (cell.mass() < CELL_MIN_SIZE + PELLET_SIZE) continue;

      Location loc = (player.target - cell.location());

      auto dir = (player.target - cell.location()).normed();

//        Agario::Velocity vel =
    }
  }

  void Engine::eat_pellets(Cell &cell) {
    auto prev_size = total_pellets();
    std::remove_if(pellets.begin(), pellets.end(),
                   [&](const Pellet &pellet) {
                     return cell > pellet && cell.collides_with(pellet);
                   });
    auto num_eaten = total_pellets() - prev_size;
    cell.increment_mass(num_eaten * PELLET_MASS);
  }

  void Engine::eat_food(Cell &cell) {
    if (cell.mass() < FOOD_MASS) return;
    auto prev_size = total_foods();
    std::remove_if(foods.begin(), foods.end(),
                   [&](const Food &food) {
                     return cell > food && cell.collides_with(food);
                   });
    auto num_eaten = total_foods() - prev_size;
    cell.increment_mass(num_eaten * FOOD_MASS);
  }

  void Engine::check_virus_collisions(Cell &cell, std::vector<Cell> &created_cells) {
    for (unsigned long i = 0; i < viruses.size(); i++) {
      Virus &virus = viruses[i];

      if (cell > virus && cell.collides_with(virus)) {

        disrupt(cell, created_cells);

        std::swap(viruses[i], viruses.back()); // O(1) removal
        viruses.pop_back();
        return; // only collide once
      }
    }
  }

  void Engine::disrupt(Cell &cell, std::vector<Cell> &created_cells) {
    Agario::mass total_mass = cell.mass(); // mass to conserve

    // reduce the cell by roughly this ratio CELL_POP_REDUCTION, making sure the
    // amount removes is divisible by CELL_POP_SIZE
    cell.reduce_mass_by_factor(CELL_POP_REDUCTION);
    cell.increment_mass((total_mass - cell.mass()) % CELL_POP_SIZE);

    Agario::mass pop_mass = total_mass - cell.mass(); // mass conservation
    int num_new_cells = div_round_up<Agario::mass>(pop_mass, CELL_POP_SIZE);

    Agario::mass remaining_mass = pop_mass;

    Agario::angle theta = cell.velocity.direction();
    for (int c = 0; c < num_new_cells; c++) {
      Agario::angle dvel_angle = cell.velocity.direction() + (2 * M_PI * c / num_new_cells);
      Agario::Velocity vel = cell.velocity + Velocity(theta + dvel_angle, CELL_POP_SPEED);

      Agario::mass new_cell_mass = std::min<Agario::mass>(remaining_mass, CELL_POP_SIZE);

      created_cells.emplace_back(cell.location(), vel, new_cell_mass);
      remaining_mass -= new_cell_mass;
    }
  }

  template <typename T>
  T random(T min, T max) {
    return (max - min) * (static_cast<T>(rand()) / static_cast<T>(RAND_MAX)) + min;
  }

  template <typename T>
  T random(T max) { return random<T>(0, max); }

  Agario::Location Engine::random_location() {
    auto x = random<distance>(canvas_width);
    auto y = random<distance>(canvas_height);
    return Location(x, y);
  }


}

