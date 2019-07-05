#pragma once

#include <vector>
#include <cstdlib>
#include <chrono>
#include <algorithm>
#include <bots/HungryShyBot.hpp>

#include "core/Player.hpp"
#include "core/settings.hpp"
#include "core/types.hpp"
#include "core/Entities.hpp"
#include "GameState.hpp"

namespace agario {

  class EngineException : public std::runtime_error {
    using runtime_error::runtime_error;
  };

  template<bool renderable>
  class Engine {
  public:

    typedef Player<renderable> Player;
    typedef Cell<renderable> Cell;
    typedef Food<renderable> Food;
    typedef Pellet<renderable> Pellet;
    typedef Virus<renderable> Virus;

    Engine(distance arena_width, distance arena_height,
           int num_pellets = DEFAULT_NUM_PELLETS, int num_viruses = DEFAULT_NUM_VIRUSES, bool pellet_regen = true) :
      state(arena_width, arena_height),
      _num_pellets(num_pellets), _num_virus(num_viruses),
      _pellet_regen(pellet_regen),
      _ticks(0), next_pid(0) {
      std::srand(std::chrono::system_clock::now().time_since_epoch().count());
    }
    Engine() : Engine(DEFAULT_ARENA_WIDTH, DEFAULT_ARENA_HEIGHT) {}

    /**
     * Total game ticks
     * @return the number of ticks that have elapsed in the game
     */
    agario::tick ticks() const { return _ticks; }
    const std::vector<Player> &players() const { return state.players; }
    const std::vector<Pellet> &pellets() const { return state.pellets; }
    const std::vector<Food> &foods() const { return state.foods; }
    const std::vector<Virus> &viruses() const { return state.viruses; }
    agario::GameState<renderable> &game_state() { return state; }
    const agario::GameState<renderable> &get_game_state() const { return state; }
    agario::distance arena_width() const { return state.arena_width; }
    agario::distance arena_height() const { return state.arena_height; }
    int total_players() { return state.players.size(); }
    int total_pellets() { return state.pellets.size(); }
    int total_viruses() { return state.viruses.size(); }
    int total_foods() { return state.foods.size(); }

    template<typename P>
    agario::pid add_player(const std::string &name) {
      auto player_ptr = std::make_shared<P>(next_pid, name, random_location());
      auto p = state.players.insert(std::make_pair(next_pid, player_ptr));
      next_pid++;
      return p.first->second->pid();
    }

    Player &player(agario::pid pid) {
      return const_cast<Player &>(get_player(pid));
    }

    const Player &get_player(agario::pid pid) const {
      if (state.players.find(pid) == state.players.end()) {
        std::stringstream ss;
        ss << "Player ID: " << pid << " does not exist.";
        throw EngineException(ss.str());
      }
      return *state.players.at(pid);
    }

    void reset() {
      state.clear();
      initialize_game();
    }

    void initialize_game() {
      add_pellets(_num_pellets);
      add_viruses(_num_virus);
    }

    void respawn(agario::pid pid) { _respawn(player(pid)); }

    /**
     * Performs a single game tick, moving all entities, performing
     * collision detection and updating the game state accordingly
     * @param elapsed_seconds the amount of time which has elapsed
     * since the previous game tick.
     */
    void tick(std::chrono::duration<double> elapsed_seconds) {
      for (auto &pair : state.players)
        tick_player(*pair.second, elapsed_seconds);

      check_player_collisions();

      move_foods(elapsed_seconds);

      if (_pellet_regen) {
        add_pellets(_num_pellets - state.pellets.size());
      }
      add_viruses(_num_virus - state.viruses.size());
      _ticks++;
    }

    Engine(const Engine &) = delete; // no copy constructor
    Engine &operator=(const Engine &) = delete; // no copy assignments
    Engine(Engine &&) = delete; // no move constructor
    Engine &operator=(Engine &&) = delete; // no move assignment

    agario::Location random_location() {
      auto x = random < agario::distance > (arena_width());
      auto y = random < agario::distance > (arena_height());
      return Location(x, y);
    }

  private:

    agario::GameState<renderable> state;

    agario::tick _ticks;
    agario::pid next_pid;

    int _num_pellets;
    int _num_virus;
    int _pellet_regen;

    /**
     * Resets a player to the starting position
     * @param pid player ID of the player to reset
     */
    void _respawn(Player &player) {
      player.kill();
      player.add_cell(random_location(), CELL_MIN_SIZE);
    }

    void add_pellets(int n) {
      for (int p = 0; p < n; p++)
        state.pellets.emplace_back(random_location());
    }

    void add_viruses(int n) {
      for (int v = 0; v < n; v++)
        state.viruses.emplace_back(random_location());
    }

    /**
     * "ticks" the given player, which involves moving the player's cells and checking
     * for collisions between the player and all other entities in the arena
     * Also performs any player actions (i.e. splitting or feeling) decrements
     * the cooldown timers on the player actions
     * @param player the player to tick
     * @param elapsed_seconds the amount of (game) time since the last game tick
     */
    void tick_player(Player &player, std::chrono::duration<double> elapsed_seconds) {

      if (_ticks % 10 == 0) // increases frame rate a lot
        player.take_action(state);

      move_player(player, elapsed_seconds);

      std::vector<Cell> created_cells;
      for (Cell &cell : player.cells) {
        eat_pellets(cell);
        eat_food(cell);
        check_virus_collisions(cell, created_cells);
      }

      maybe_emit_food(player);
      maybe_split(player, created_cells);

      // add any cells that were created
      player.add_cells(created_cells);
      created_cells.erase(created_cells.begin(), created_cells.end());

      recombine_cells(player);
    }

    /**
     * Moves all of the cells of the given player by an amount proportional
     * to the elapsed time since the last tick, given by elapsed_seconds
     * @param player the player to move
     * @param elapsed_seconds time since the last game tick
     */
    void move_player(Player &player, std::chrono::duration<double> elapsed_seconds) {
      auto dt = elapsed_seconds.count();

      for (auto &cell : player.cells) {
        cell.velocity.dx = 3 * (player.target.x - cell.x);
        cell.velocity.dy = 3 * (player.target.y - cell.y);

        // clip speed
        auto speed_limit = max_speed(cell.mass());
        cell.velocity.clamp_speed(0, speed_limit);

        cell.move(dt);
        cell.splitting_velocity.decelerate(SPLIT_DECELERATION, dt);

        check_boundary_collisions(cell);
      }

      // make sure not to move two of players own cells into one another
      check_player_self_collisions(player);
    }

    void move_foods(std::chrono::duration<double> elapsed_seconds) {
      auto dt = elapsed_seconds.count();

      for (auto &food : state.foods) {
        if (food.velocity.magnitude() == 0) continue;

        food.decelerate(FOOD_DECEL, dt);
        food.move(dt);

        check_boundary_collisions(food);
      }
    }

    /**
     * Constrains the location of `ball` to be inside the boundaries
     * of the arena
     * @param ball the ball to keep inside the arena
     */
    void check_boundary_collisions(Ball &ball) {
      ball.x = clamp<agario::distance>(ball.x, 0, arena_width());
      ball.y = clamp<agario::distance>(ball.y, 0, arena_height());
    }

    /**
     * Moves all of `player`'s cells apart slightly such that
     * cells which aren't eligible for recombining don't overlap
     * with other cells of the same player.
     */
    void check_player_self_collisions(Player &player) {
      for (auto it = player.cells.begin(); it != player.cells.end(); ++it) {
        for (auto it2 = std::next(it); it2 != player.cells.end(); it2++) {

          // only allow collisions if both are eligible for recombining
          if (it->can_recombine() && it2->can_recombine())
            continue;

          Cell &cell_a = *it;
          Cell &cell_b = *it2;
          if (cell_a.touches(cell_b))
            prevent_overlap(cell_a, cell_b);
        }
      }
    }

    /**
     * moves `cell_a` and `cell_b` apart slightly
     * such that they cannot be overlapping
     * @param cell_a first cell to move apart
     * @param cell_b second cell to move apart
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

    /**
     * checks for collisions between the given cell and
     * all of the pellets in the game, removing those pellets
     * from the game which the cell eats
     * @param cell the cell which is doing the eating
     */
    void eat_pellets(Cell &cell) {
      auto prev_size = total_pellets();

      state.pellets.erase(
        std::remove_if(state.pellets.begin(), state.pellets.end(),
                       [&](const Pellet &pellet) {
                         return cell.can_eat(pellet) && cell.collides_with(pellet);
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
                         return cell.can_eat(pellet) && cell.collides_with(pellet);
                       }),
        state.foods.end());
      auto num_eaten = prev_size - total_foods();
      cell.increment_mass(num_eaten * FOOD_MASS);
    }

    void emit_foods(Player &player) {

      // emit one pellet from each sufficiently large cell
      for (Cell &cell : player.cells) {

        // not big enough to emit pellet
        if (cell.mass() < CELL_MIN_SIZE + FOOD_MASS) continue;

        auto dir = (player.target - cell.location()).normed();
        Location loc = cell.location() + dir * cell.radius();

        Velocity vel(dir * FOOD_SPEED);
        Food food(loc, vel);

        state.foods.emplace_back(std::move(food));
        cell.increment_mass(-food.mass());
      }
    }

    void maybe_emit_food(Player &player) {
      if (player.feed_cooldown > 0)
        player.feed_cooldown -= 1;

      if (player.action == agario::action::feed && player.feed_cooldown == 0) {
        emit_foods(player);
        player.feed_cooldown = 10;
      }
    }

    void maybe_split(Player &player, std::vector<Cell> &created_cells) {
      if (player.split_cooldown > 0)
        player.split_cooldown -= 1;

      if (player.action == agario::action::split && player.split_cooldown == 0) {
        player_split(player, created_cells);
        player.split_cooldown = 30;
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
        new_cell.splitting_velocity = vel;

        cell.reset_recombine_timer();
        new_cell.reset_recombine_timer();

        created_cells.emplace_back(std::move(new_cell));
      }
    }

    /**
     * Checks all pairs of players for collisions that result
     * in one cell eating another. Updates the corresponding lists
     * of cells in each player to reflect any collisions.
     */
    void check_player_collisions() {
      for (auto p1_it = state.players.begin(); p1_it != state.players.end(); ++p1_it)
        for (auto p2_it = std::next(p1_it); p2_it != state.players.end(); ++p2_it)
          check_players_collisions(*p1_it->second, *p2_it->second);
    }

    /**
     * Checks cell-cell collisions between players `p1` and `p2`
     * and does consumptions/removal of cells that collide
     * @param p1 The first players
     * @param p2 The second player
     */
    void check_players_collisions(Player &p1, Player &p2) {
      for (auto &cell : p2.cells)
        eat_others(p1, cell);
      for (auto &cell : p1.cells)
        eat_others(p2, cell);
    }

    /**
     * Checks if `cell` collides with and can eat any of `player`'s
     * cells. Updates the mas of `cell` and removes cells from
     * `player` if any are eaten.
     * todo: update this so that removals are O(1) making this
     * section O(n) rather tha O(n^2)
     */
    void eat_others(Player &player, Cell &cell) {

      agario::mass original_mass = player.mass();

      // remove all the cells that we eat
      player.cells.erase(
        std::remove_if(player.cells.begin(), player.cells.end(),
                       [&](const Cell &other_cell) {
                         return cell.collides_with(other_cell) && cell.can_eat(other_cell);
                       }),
        player.cells.end());

      agario::mass gained_mass = original_mass - player.mass();
      cell.increment_mass(gained_mass);
    }

    void recombine_cells(Player &player) {
      for (auto it = player.cells.begin(); it != player.cells.end(); ++it) {
        if (!it->can_recombine()) continue;

        Cell &cell = *it;

        for (auto it2 = std::next(it); it2 != player.cells.end();) {
          Cell &other = *it2;
          if (other.can_recombine() && cell.collides_with(other)) {
            cell.increment_mass(other.mass());
            it2 = player.cells.erase(it2);
          } else {
            ++it2;
          }
        }
      }
    }

    void check_virus_collisions(Cell &cell, std::vector<Cell> &created_cells) {
      for (auto it = state.viruses.begin(); it != state.viruses.end();) {
        Virus &virus = *it;

        if (cell.can_eat(virus) && cell.collides_with(virus)) {
          disrupt(cell, virus, created_cells);
          std::swap(*it, state.viruses.back()); // O(1) removal
          state.viruses.pop_back();
          return; // only collide once
        } else ++it;
      }
    }

    void disrupt(Cell &cell, Virus &virus, std::vector<Cell> &created_cells) {
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
        agario::Velocity vel = Velocity(theta + dvel_angle, max_speed(CELL_POP_SIZE));

        agario::mass new_cell_mass = std::min<agario::mass>(remaining_mass, CELL_POP_SIZE);

        auto loc = virus.location();
        Cell new_cell(loc, cell.velocity, new_cell_mass);
        new_cell.splitting_velocity = vel;
        new_cell.reset_recombine_timer();

        created_cells.emplace_back(std::move(new_cell));
        remaining_mass -= new_cell_mass;
      }
      cell.reset_recombine_timer();
    }

    float split_speed(agario::mass mass) {
      return clamp(3 * (std::pow(max_speed(mass), 1.2)), 20.0, 130.0);
    }

    float max_speed(agario::mass mass) {
      return CELL_MAX_SPEED / std::sqrt((float) mass);
    }

    template<typename T>
    T random(T min, T max) {
      return (max - min) * (static_cast<T>(rand()) / static_cast<T>(RAND_MAX)) + min;
    }

    template<typename T>
    T random(T max) { return random<T>(0, max); }

  };

}

