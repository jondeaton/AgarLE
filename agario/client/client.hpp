#pragma once

#include "rendering/renderer.hpp"
#include "rendering/window.hpp"

#include "core/renderables.hpp"
#include "engine/Engine.hpp"

#include "bots/bots.hpp"

#include <chrono>

#include <string>
#include <ctime>
#include <memory>
#include <bots/HungryBot.hpp>

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
      return engine.add_player<Player>(name);
    }

    void add_bots() {
      for (int i = 0; i < 10; i++)
        engine.add_player<bot::HungryBot<true>>("hungry");

      for (int i = 0; i < 25; i++)
        engine.add_player<bot::HungryShyBot<true>>("shy");
    }

    void initialize_renderer() {
      window = std::make_shared<agario::Window>();
      renderer = std::make_unique<agario::Renderer>(window,
                                                    engine.arena_width(),
                                                    engine.arena_height());
    }

    void game_loop() {
      if (renderer == nullptr) initialize_renderer();

      auto beginning = std::chrono::system_clock::now();
      int count = 0;

      auto before = std::chrono::system_clock::now();
      while (!window->should_close()) {

        auto &player = engine.player(player_pid);

        if (player.dead()) {
          std::cout << "Player \"" << player.name() << "\" (pid ";
          std::cout << player.pid() << ") died." << std::endl;
          engine.reset_player(player_pid);
        }

        process_input();
        renderer->render_screen(player, engine.game_state());

        auto now = std::chrono::system_clock::now();
        auto dt = now - before;

        count++;
        engine.tick(dt);

        before = std::chrono::system_clock::now();
      }
      auto now = std::chrono::system_clock::now();
      std::chrono::duration<double> total_time = now - beginning;

      float fps = count / total_time.count();
      std::cout << "Average FPS: " << fps << std::endl;
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
