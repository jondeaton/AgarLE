#pragma once

#include "rendering/renderer.hpp"
#include "rendering/window.hpp"

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
      server(), port(),
      engine(DEFAULT_ARENA_WIDTH, DEFAULT_ARENA_HEIGHT),
      renderer(nullptr) {
      engine.initialize_game();
    }

    Client(std::string server, int port) :
      server(std::move(server)), port(port), renderer(nullptr) {}

    void connect() {
      std::cout << "Connecting to: " << server << ":" << port << "..." << std::endl;
      // todo: set arena_height and width form server (among other things)
      std::cout << "remote server connection not implemented yet." << std::endl;

    }

    template<typename... Args>
    void set_player(agario::pid pid) {
      player_pid = pid;
    }

    agario::pid add_player(const std::string &name) {
      auto pid = engine.add_player(name);
      return pid;
    }

    void initialize_renderer() {
      window = std::make_shared<agario::Window>();
      renderer = std::make_unique<agario::Renderer>(window,
                                                    engine.arena_width(),
                                                    engine.arena_height());
    }

    void game_loop(std::optional<int> num_iterations = std::nullopt) {
      if (renderer == nullptr) initialize_renderer();

      double fps = 0;
      int fps_count = 0;

      auto before = std::chrono::system_clock::now();
      while ((!num_iterations || num_iterations > 0) && !window->should_close()) {

        auto &player = engine.player(player_pid);

        process_input();
        renderer->render_screen(player, engine.game_state());

        auto now = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = now - before;

        fps += 1 / elapsed_seconds.count();
        fps_count += 1;

        engine.tick(elapsed_seconds);

        before = std::chrono::system_clock::now();
        if (num_iterations) (*num_iterations)--;
      }
      std::cout << "Average FPS: " << (fps / fps_count) << std::endl;

      renderer->terminate();
    }

  private:

    std::string server;
    int port;

    agario::pid player_pid; // pid of the player we're tracking

    agario::Engine<true> engine;

    // todo: change these into regular pointers?
    std::unique_ptr<agario::Renderer> renderer;
    std::shared_ptr<agario::Window> window;

    void process_input() {
      GLFWwindow *win = window->pointer();
      auto &player = engine.player(player_pid);

      double xpos, ypos;
      glfwGetCursorPos(win, &xpos, &ypos);
      player.target = renderer->to_target(player, xpos, ypos);
      player.action = agario::action::none;

      if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(win, true);

      if (glfwGetKey(win, GLFW_KEY_SPACE) == GLFW_PRESS)
        player.action = agario::action::split;

      if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
        player.action = agario::action::feed;
    }

  };

}
