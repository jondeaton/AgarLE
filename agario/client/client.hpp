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

    void connect() { }

    template<typename... Args>
    void set_player(Args&&... args) {
      _player = std::make_shared<Player>(std::forward<Args>(args)...);
      renderer.player = _player;
    }

    template<typename... Args>
    void add_player(Args&&... args) {
      players.emplace_back(std::forward<Args>(args)...);
    }

    void game_loop(int num_iterations=-1) {

      while (num_iterations != 0 && renderer.ready()) {
//      process_input(window);
        renderer.render_screen(*_player, players, foods, pellets, viruses);

        // todo: emit "heartbeat" signal to server
        if (num_iterations > 0) num_iterations--;
      }
      renderer.terminate();
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
    Agario::Renderer renderer;

    std::clock_t g_PreviousTicks;
    std::clock_t g_CurrentTicks;

    std::shared_ptr<Player> _player;

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
