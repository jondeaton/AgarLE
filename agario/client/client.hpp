#pragma once

#include "renderer.hpp"
#include "renderables.hpp"

#include <string>
#include <ctime>

void process_input(GLFWwindow *window);

class AgarioClient {
public:

  void connect() { }


  void game_loop(int num_iterations=-1) {

    while (num_iterations != 0 && renderer.ready()) {
//      process_input(window);
      renderer.render_screen(_player, players, foods, pellets, viruses);

      // emit "heartbeat" signal to server
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
  AgarioRenderer renderer;

  std::clock_t g_PreviousTicks;
  std::clock_t g_CurrentTicks;

  player _player;
  std::vector<player> players;
  std::vector<food> foods;
  std::vector<pellet> pellets;
  std::vector<virus> viruses;
};

// Handle key input
void process_input(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    std::cout << "Space bar pressed!" << std::endl;
}