#pragma once

#include "rendering/renderer.hpp"
#include "core/renderables.hpp"
#include "engine/Engine.hpp"

#include <chrono>

#include <string>
#include <ctime>
#include <memory>

void process_input(GLFWwindow *window);

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
    void set_player(Args &&... args) {
      player = std::make_shared<Player>(std::forward<Args>(args)...);
      if (renderer != nullptr)
        renderer->player = player;
    }

    template<typename... Args>
    void add_player(Args &&... args) {
      players.emplace_back(std::forward<Args>(args)...);
    }

    void initialize_renderer() {
      agario::distance arena_width = 1000;
      agario::distance arena_height = 1000;
      renderer = std::make_unique<agario::Renderer>(arena_width, arena_height);
      renderer->player = player;
    }

    void game_loop(std::optional<int> num_iterations = std::nullopt) {
      if (renderer == nullptr) initialize_renderer();

      auto before = std::chrono::system_clock::now();
      while ((!num_iterations || num_iterations > 0) && renderer->ready()) {
//      process_input(window);
        renderer->render_screen(players, foods, pellets, viruses);

        auto now = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = now - before;
        engine.tick(elapsed_seconds);
        before = std::chrono::system_clock::now();

        if (num_iterations) (*num_iterations)--;
      }
      renderer->terminate();
    }

  private:

    std::string server;
    int port;

    agario::Engine<true> engine;
    std::unique_ptr<agario::Renderer> renderer;
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
