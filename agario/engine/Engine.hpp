#pragma once

#include <vector>
#include <cstdlib>
#include <chrono>
#include <algorithm>

#include "core/Player.hpp"
#include "settings.hpp"
#include "core/types.hpp"
#include "core/Entities.hpp"

namespace agario {

  class EngineException : public std::runtime_error {
    using runtime_error::runtime_error;
  };

  template<bool renderable>
  class Engine {
  public:

    typedef Player <renderable> Player;
    typedef Cell <renderable> Cell;
    typedef Food <renderable> Food;
    typedef Pellet <renderable> Pellet;
    typedef Virus <renderable> Virus;

    explicit Engine(distance arena_width, distance arena_height) :
      _arena_width(arena_width), _arena_height(arena_height),
      ticks(0), next_pid(0) {
      std::srand(std::chrono::system_clock::now().time_since_epoch().count());
    }

    Engine() : Engine(DEFAULT_ARENA_WIDTH, DEFAULT_ARENA_HEIGHT) {}

    agario::pid add_player(const std::string &name) {
      std::pair<typename std::unordered_map<agario::pid, Player>::iterator, bool> p;
      p = state.players.emplace(std::piecewise_construct,
                                std::forward_as_tuple(next_pid),
                                std::forward_as_tuple(next_pid, name, random_color()));

      if (!p.second) {
        std::stringstream ss;
        ss << "Could not insert player named " << name << "at pid " << next_pid;
        throw EngineException(ss.str().c_str());
      }

      next_pid++;
      return p.first->second.pid();
    }

    Player &player(agario::pid pid) {
      return state.players.at(pid);
    }

    void initialize_game() {
      add_pellets(1000);
      add_viruses(25);
    }

    const std::vector<Player> &players() const { return state.players; }

    const std::vector<Pellet> &pellets() const { return state.pellets; }

    const std::vector<Food> &foods() const { return state.foods; }

    const std::vector<Virus> &viruses() const { return state.viruses; }

    agario::GameState<renderable> &game_state() {
      return state;
    }

    agario::distance arena_height() const { return _arena_height; }

    agario::distance arena_width() const { return _arena_width; }

    void tick(std::chrono::duration<double> elapsed_seconds) {
      for (auto &pair : state.players)
        tick_player(pair.second, elapsed_seconds);
      ticks++;
    }

    void move_entities(std::chrono::duration<double> elapsed_seconds) {
      for (auto &pair : state.players)
        move_player(pair.second, elapsed_seconds);
      // todo: move other entities
    }

    template<typename T>
    int sign(T val) {
      if (val == T(0)) return 0;
      return val > 0 ? 1 : -1;
    }

    void move_player(Player &player, std::chrono::duration<double> elapsed_seconds) {
      auto dt = elapsed_seconds.count();

      for (auto &cell : player.cells) {
        cell.velocity.dx = 3 * (player.target.x - cell.x);
        cell.velocity.dy = 3 * (player.target.y - cell.y);

        // clip speed
        auto speed_limit = max_speed(cell.mass());
        if (cell.speed() > speed_limit)
          cell.velocity.set_speed(speed_limit);

        cell.x += (cell.velocity.dx + cell.splitting_velocity.dx) * dt;
        cell.y += (cell.velocity.dy + cell.splitting_velocity.dy) * dt;

        cell.splitting_velocity.dx *= 0.95;
        cell.splitting_velocity.dy *= 0.95;

        check_boundary_collisions(cell);
      }

      // make sure not to move two of players own cells into one another
      check_player_self_collisions(player);
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

    distance _arena_width;
    distance _arena_height;

    agario::tick ticks;
    agario::pid next_pid;

    void add_pellets(int num_pellets) {
      for (int p = 0; p < num_pellets; p++)
        state.pellets.emplace_back(random_location());
    }

    void add_viruses(int num_virus) {
      for (int v = 0; v < num_virus; v++)
        state.viruses.emplace_back(random_location());
    }

    void tick_player(Player &player, std::chrono::duration<double> elapsed_seconds) {
      move_player(player, elapsed_seconds);

      std::vector<Cell> created_cells;
      for (Cell &cell : player.cells) {
        eat_pellets(cell);
        eat_food(cell);
//        check_virus_collisions(cell, created_cells);
      }

      // handle emitting food
//      if (player.action == agario::action::feed)
//        emit_foods(player);

      // handle splitting
      if (player.split_cooldown > 0)
        player.split_cooldown -= 1;

      if (player.action == agario::action::split && player.split_cooldown == 0) {
        player_split(player, created_cells);
        player.split_cooldown = 30;
      }

      // add any cells that were created
      player.cells.insert(std::end(player.cells),
                          std::make_move_iterator(created_cells.begin()),
                          std::make_move_iterator(created_cells.end()));
      created_cells.erase(created_cells.begin(), created_cells.end());

      // player collisions
//      check_player_collisions(player);

//      recombine_cells(player);
      // todo: decrement recombine timers
      // todo: increment or decrement entity speeds
      // todo: reset player action?
      // todo: player dead if has zero cells remaining
    }

    void check_boundary_collisions(Cell &cell) {
      // stay inside arena
      if (cell.x < 0) cell.x = 0;
      if (cell.x > _arena_width) cell.x = _arena_width;
      if (cell.y < 0) cell.y = 0;
      if (cell.y > _arena_height) cell.y = _arena_height;

    }

    void check_player_self_collisions(Player &player) {
      for (auto it = player.cells.begin(); it != player.cells.end(); ++it) {
        if (it->can_recombine()) continue;
        for (auto it2 = std::next(it); it2 != player.cells.end(); it2++) {
          Cell &cell_a = *it;
          Cell &cell_b = *it2;
          if (cell_a.touches(cell_b))
            prevent_overlap(cell_a, cell_b);
        }
      }
    }

    /**
     * moves `cell_a` and `cell_b` appart slightly
     * such that they cannot be overlapping
     * @param cell_a first cell to move apart
     * @param cell_b second cell to move appart
     */
    void prevent_overlap(Cell &cell_a, Cell &cell_b) {

      auto dx = cell_b.x - cell_a.x;
      auto dy = cell_b.y - cell_a.y;

      auto dist = sqrt(dx * dx + dy * dy);
      auto target_dist = cell_a.radius() + cell_b.radius();

      if (dist > target_dist) return; // aren't overlapping

      auto x_ratio = dx / (std::abs(dx) + std::abs(dy));
      auto y_ratio = dy / (std::abs(dx) + std::abs(dy));

      cell_b.x += (target_dist - dist) * x_ratio / 2;
      cell_b.y += (target_dist - dist) * y_ratio / 2;

      cell_a.x -= (target_dist - dist) * x_ratio / 2;
      cell_a.y -= (target_dist - dist) * y_ratio / 2;
    }

    void eat_pellets(Cell &cell) {
      auto prev_size = total_pellets();

      state.pellets.erase(
        std::remove_if(state.pellets.begin(), state.pellets.end(),
                       [&](const Pellet &pellet) {
                         return cell > pellet && cell.collides_with(pellet);
                       }),
        state.pellets.end());
      auto num_eaten = prev_size - total_pellets();
      cell.increment_mass(num_eaten * PELLET_MASS);
    }

    void eat_food(Cell &cell) {
      if (cell.mass() < FOOD_MASS) return;
      auto prev_size = total_foods();

      state.foods.erase(
        std::remove_if(state.foods.begin(), state.foods.end(),
                       [&](const Food &pellet) {
                         return cell > pellet && cell.collides_with(pellet);
                       }),
        state.foods.end());
      auto num_eaten = prev_size - total_foods();
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

        if (cell.mass() < CELL_SPLIT_MINIMUM) continue;

        agario::mass split_mass = cell.mass() / 2;
        auto remaining_mass = cell.mass() - split_mass;

        cell.set_mass(remaining_mass);

        auto dir = (player.target - cell.location()).normed();
        Location loc = cell.location() + dir * cell.radius();
        Velocity vel(dir * split_speed(split_mass));

        // todo: add constructor that takes splitting velocity (and color)
        Cell new_cell(loc, vel, split_mass);
        new_cell.set_color(player.color());
        new_cell.splitting_velocity = vel;

        cell.reset_recombine_timer();
        new_cell.reset_recombine_timer();

        created_cells.emplace_back(std::move(new_cell));
      }
    }

    void check_player_collisions(Player &player) {
      for (Cell &cell : player.cells)
        eat_others(player, cell);
    }

    void eat_others(Player &player, Cell &cell) {

      agario::mass gained_mass = 0;

      for (auto &pair : state.players) {
        auto &other_player = pair.second;
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

    float split_speed(agario::mass mass) {
      return 3 * max_speed(mass);
    }

    float max_speed(agario::mass mass) {
      return CELL_MAX_SPEED / sqrt((float) mass);
    }

    agario::Location random_location() {
      auto x = random < agario::distance > (_arena_width);
      auto y = random < agario::distance > (_arena_height);
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

