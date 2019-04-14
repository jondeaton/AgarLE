#pragma once

#include "rendering/renderer.hpp"
#include "core/renderables.hpp"
#include "engine/Engine.hpp"

#include <chrono>

#include <string>
#include <ctime>
#include <memory>

namespace agario {

  class Client {
  public:
    typedef Player<true> Player;
    typedef Cell<true> Cell;
    typedef Pellet<true> Pellet;
    typedef Food<true> Food;
    typedef Virus<true> Virus;

    Client() :
      server(), port(), renderer(nullptr) {}

    Client(std::string server, int port) :
      server(std::move(server)), port(port), renderer(nullptr) {}

    void connect() {
      std::cout << "Connecting to: " << server << ":" << port << "..." << std::endl;
      // todo
      std::cout << "remote server connection not implemented yet." << std::endl;
    }

    template<typename... Args>
    void set_player(agario::pid pid) {
      player_pid = pid;
    }

    agario::pid add_player(const std::string &name) {
      return engine.add_player(name);
    }

    void initialize_renderer() {
      agario::distance arena_width = 1000;
      agario::distance arena_height = 1000;
      renderer = std::make_unique<agario::Renderer>(arena_width, arena_height);
    }

    void game_loop(std::optional<int> num_iterations = std::nullopt) {
      if (renderer == nullptr) initialize_renderer();

      double fps = 0;
      int count = 0;

      auto before = std::chrono::system_clock::now();
      while ((!num_iterations || num_iterations > 0) && renderer->ready()) {

        auto &player = engine.player(player_pid);

        process_input(renderer->window, player);
        renderer->render_screen(player, engine.game_state());

        auto now = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = now - before;

        fps += 1 / elapsed_seconds.count();
        count += 1;

        engine.move_entities(elapsed_seconds);

        before = std::chrono::system_clock::now();
        if (num_iterations) (*num_iterations)--;
      }
      std::cout << "Average FPS: " << (fps / count) << std::endl;

      renderer->terminate();
    }

  private:

    std::string server;
    int port;

    agario::pid player_pid; // pid of the player we're tracking

    agario::Engine<true> engine;
    std::unique_ptr<agario::Renderer> renderer;

    void process_input(GLFWwindow *window, Player &player) {

      double xpos, ypos;
      glfwGetCursorPos(window, &xpos, &ypos);
      player.target = renderer->to_target(xpos, ypos);

      if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

      if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        std::cout << "Space bar pressed!" << std::endl;
    }

  };

}
