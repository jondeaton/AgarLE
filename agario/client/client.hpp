#pragma once

#include "renderer.hpp"
#include "core/renderables.hpp"

#include <string>
#include <ctime>
#include <memory>

void process_input(GLFWwindow *window);

namespace Agario {

  class Client {
  public:
    typedef Player<true> Player;
    typedef Cell<true> Cell;
    typedef Pellet<true> Pellet;
    typedef Food<true> Food;
    typedef Virus<true> Virus;


    explicit Client() : renderer(nullptr), g_PreviousTicks(0), g_CurrentTicks(0) { }

    void connect() { }

    template<typename... Args>
    void set_player(Args&&... args) {
      player = std::make_shared<Player>(std::forward<Args>(args)...);
      if (renderer != nullptr)
        renderer->player = player;
    }

    template<typename... Args>
    void add_player(Args&&... args) {
      players.emplace_back(std::forward<Args>(args)...);
    }

    void initialize_renderer() {
      Agario::distance arena_width = 1000;
      Agario::distance arena_height = 1000;
      // todo: get arena dimensions from
      renderer = std::make_unique<Agario::Renderer>(arena_width, arena_height);
      renderer->player = player;
    }

    void game_loop(std::optional<int> num_iterations=std::nullopt) {
      if (renderer == nullptr) initialize_renderer();

      while ((!num_iterations || num_iterations > 0) && renderer->ready()) {

//      process_input(window);
        renderer->render_screen(*player, players, foods, pellets, viruses);

        // todo: emit "heartbeat" signal to server
        if (num_iterations) (*num_iterations)--;
      }
      renderer->terminate();
    }

    // todo: change these over to
    void init_player(unsigned int pid, unsigned int mass) {
//    _player.assign_pid(pid);
//    _player.set_mass(mass)
    }

    // socket.on('serverTellPlayerMove', function (userData, foodsList, massList, virusList) {
    void server_tell_player_move() {
      // todo: yeah... gotta figure this one out
    }

  private:
    std::unique_ptr<Agario::Renderer> renderer;

    std::clock_t g_PreviousTicks;
    std::clock_t g_CurrentTicks;

    std::shared_ptr<Player> player;

    std::vector<Player> players;
    std::vector<Food> foods;
    std::vector<Pellet> pellets;
    std::vector<Virus> viruses;
  };

// Handle key input
  void process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
      std::cout << "Space bar pressed!" << std::endl;
  }

}
