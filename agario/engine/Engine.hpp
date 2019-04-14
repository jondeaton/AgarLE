#pragma once

#include <vector>
#include <cstdlib>
#include <chrono>

#include "core/Player.hpp"
#include "settings.hpp"
#include "core/types.hpp"
#include "core/Entities.hpp"

namespace agario {

  template<bool renderable>
  class Engine {
  public:

    typedef Player <renderable> Player;
    typedef Cell <renderable> Cell;
    typedef Food <renderable> Food;
    typedef Pellet <renderable> Pellet;
    typedef Virus <renderable> Virus;

    Engine() :
      arena_width(DEFAULT_ARENA_WIDTH), arena_height(DEFAULT_ARENA_HEIGHT),
      ticks(0) {
      std::srand(std::chrono::system_clock::now().time_since_epoch().count());
    }

    explicit Engine(distance arena_width, distance arena_height) :
      arena_width(arena_width), arena_height(arena_height), ticks(0) {
      std::srand(std::chrono::system_clock::now().time_since_epoch().count());
    }

    const std::vector<Player> &players() const { return state.players; }

    const std::vector<Pellet> &pellets() const { return state.pellets; }

    const std::vector<Food> &foods() const { return state.foods; }

    const std::vector<Virus> &viruses() const { return state.viruses; }

    const std::vector<Player> &leaderboard() {
      std::sort(std::begin(state.players), std::end(state.players));
      return players;
    }

    agario::GameState<renderable> &game_state() {
      return state;
    }

    void tick(std::chrono::duration<double> elapsed_seconds) {
      (void) elapsed_seconds;

      std::vector<Food> created_masses;
      for (Player &player : state.players)
        tick_player(player);
      ticks++;
    }

    void move_entities(std::chrono::duration<double> elapsed_seconds) {
      for (auto &player : state.players)
        move_player(player, elapsed_seconds);
      // todo: move other entities
    }

    void move_player(Player &player, std::chrono::duration<double> elapsed_seconds) {
      (void) player;

      for (auto &cell : player.cells) {
        cell.velocity.dx = player.target.x * 10;
        cell.velocity.dy = player.target.y * 10;

        cell.x += cell.velocity.dx * elapsed_seconds.count();
        cell.y += cell.velocity.dy * elapsed_seconds.count();

        // stay inside arena
        if (cell.x < 0) cell.x = 0;
        if (cell.x > arena_width) cell.x = arena_width;
        if (cell.y < 0) cell.y = 0;
        if (cell.x > arena_height) cell.y = arena_height;
      }

      // make sure not to move two of players own cells into one another
    }

    int total_players() { return state.players.size(); }

    int total_pellets() { return state.pellets.size(); }

    int total_viruses() { return state.viruses.size(); }

    int total_foods() { return state.foods.size(); }

    Engine(const Engine &) = delete; // no copy constructor
    Engine &operator=(const Engine &) = delete; // no copy assignments
    Engine(Engine &&) = delete; // no move constructor
    Engine &operator=(Engine &&) = delete; // no move assignment

  private:

    agario::GameState<renderable> state;

    distance arena_width;
    distance arena_height;

    agario::tick ticks;

    void add_pellets(int num_pellets) {
      while (num_pellets > 0) {
//      Agario::Location loc = random_location();
//      Agario::Pellet pellet(loc);

        state.pellets.emplace_back(random_location());
        num_pellets--;
      }
    }

    void add_virus(int num_virus) {
      while (num_virus > 0) {
        state.viruses.emplace_back(random_location());
        num_virus--;
      }
    }

    void tick_player(Player &player, std::chrono::duration<double> elapsed_seconds) {
      move_player(player, elapsed_seconds);

      std::vector<Cell> created_cells;
      for (Cell &cell : player.cells) {
        eat_pellets(cell);
        eat_food(cell);

        // todo: change VIRUS_SIZE to static member of Virus?
        check_virus_collisions(cell, created_cells);

        if (player.action == agario::action::feed)
          emit_foods(player);

        if (player.action == agario::action::split)
          player_split(player, created_cells);

      }

      // add any cells that were created
      player.cells.insert(std::end(player.cells),
                          std::begin(created_cells),
                          std::end(created_cells));

      // player collisions
      check_player_collisions(player);

      recombine_cells(player);
      // todo: recombine cells
      // todo: decrement recombine timers
      // todo: increment or decrement entity speeds
      // todo: reset player action?
      // todo: player dead if has zero cells remaining
    }

    void eat_pellets(Cell &cell) {
      auto prev_size = total_pellets();

      // todo: fix remove_if
      std::remove_if(state.pellets.begin(), state.pellets.end(),
                     [&](const Pellet &pellet) {
                       return cell > pellet && cell.collides_with(pellet);
                     });
      auto num_eaten = total_pellets() - prev_size;
      cell.increment_mass(num_eaten * PELLET_MASS);
    }

    void eat_food(Cell &cell) {
      if (cell.mass() < FOOD_MASS) return;
      auto prev_size = total_foods();

      // todo: fix remove_if
      std::remove_if(state.foods.begin(), state.foods.end(),
                     [&](const Food &food) {
                       return cell > food && cell.collides_with(food);
                     });
      auto num_eaten = total_foods() - prev_size;
      cell.increment_mass(num_eaten * FOOD_MASS);
    }

    // actions
    void emit_foods(Player &player) {

      // emit one pellet from each sufficiently large cell
      for (Cell &cell : player.cells) {

        // not big enough to emit pellet
        if (cell.mass() < CELL_MIN_SIZE + PELLET_SIZE) continue;

        auto dir = (player.target - cell.location()).normed();
        Location loc = cell.location() + dir * cell.radius();

        Velocity vel(dir * FOOD_SPEED);

        state.foods.emplace_back(loc, vel);
      }
    }

    void player_split(Player &player, std::vector<Cell> &created_cells) {

      for (Cell &cell : player.cells) {

        if (cell.mass() < CELL_MIN_SIZE * 2) continue;

        agario::mass split_mass = cell.mass() / 2;
        auto remaining_mass = cell.mass() - split_mass;

        cell.set_mass(remaining_mass);

        auto dir = (player.target - cell.location()).normed();
        Location loc = cell.location() + dir * cell.radius();

        Velocity vel(dir * 2 * max_speed(split_mass));

        created_cells.emplace_back(loc, vel, split_mass);
      }
    }


    void check_player_collisions(Player &player) {
      for (Cell &cell : player.cells)
        eat_others(player, cell);
    }

    void eat_others(Player &player, Cell &cell) {

      agario::mass gained_mass = 0;

      for (Player &other_player : state.players) {
        if (other_player == player) continue;
        for (Cell &other_cell : other_player.cells) {
          if (cell.collides_with(other_cell) && cell > other_cell)
            gained_mass += other_cell.mass();
        }

        // remove all the cells that we eat
        other_player.cells.erase(
          std::remove_if(other_player.cells.begin(), other_player.cells.end(),
                         [&](const Cell &other_cell) {
                           return cell.collides_with(other_cell) && cell > other_cell;
                         }),
          other_player.cells.end());
      }

      cell.increment_mass(gained_mass);
    }

    void recombine_cells(Player &player) {
      for (auto cell = player.cells.begin(); cell != player.cells.end(); ++cell) {
        agario::mass gained_mass = 0;
        (void) gained_mass;
        for (auto other_cell = cell + 1; other_cell != player.cells.end(); ++other_cell) {
//        if (cell->collides_with(*other_cell))
//          gained_mass += other_cell->mass();

// todo: uhh this is more complicated than anticipated...
        }
      }

    }

    void check_virus_collisions(Cell &cell, std::vector<Cell> &created_cells) {
      for (unsigned long i = 0; i < state.viruses.size(); i++) {
        Virus &virus = state.viruses[i];

        if (cell > virus && cell.collides_with(virus)) {

          disrupt(cell, created_cells);

          std::swap(state.viruses[i], state.viruses.back()); // O(1) removal
          state.viruses.pop_back();
          return; // only collide once
        }
      }
    }

    void disrupt(Cell &cell, std::vector<Cell> &created_cells) {
      agario::mass total_mass = cell.mass(); // mass to conserve

      // reduce the cell by roughly this ratio CELL_POP_REDUCTION, making sure the
      // amount removes is divisible by CELL_POP_SIZE
      cell.reduce_mass_by_factor(CELL_POP_REDUCTION);
      cell.increment_mass((total_mass - cell.mass()) % CELL_POP_SIZE);

      agario::mass pop_mass = total_mass - cell.mass(); // mass conservation
      int num_new_cells = div_round_up<agario::mass>(pop_mass, CELL_POP_SIZE);

      agario::mass remaining_mass = pop_mass;

      agario::angle theta = cell.velocity.direction();
      for (int c = 0; c < num_new_cells; c++) {
        agario::angle dvel_angle = cell.velocity.direction() + (2 * M_PI * c / num_new_cells);
        agario::Velocity vel = cell.velocity + Velocity(theta + dvel_angle, CELL_POP_SPEED);

        agario::mass new_cell_mass = std::min<agario::mass>(remaining_mass, CELL_POP_SIZE);

        auto loc = cell.location();
        created_cells.emplace_back(loc, vel, new_cell_mass);
        remaining_mass -= new_cell_mass;
      }
    }

    float max_speed(agario::mass mass) {
      return CELL_MAX_SPEED * CELL_MIN_SIZE / (float) mass;
    }

    agario::Location random_location() {
      auto x = random < distance > (arena_width);
      auto y = random < distance > (arena_height);
      return Location(x, y);
    }

    template<typename T>
    T random(T min, T max) {
      return (max - min) * (static_cast<T>(rand()) / static_cast<T>(RAND_MAX)) + min;
    }

    template<typename T>
    T random(T max) { return random<T>(0, max); }

  };
}

