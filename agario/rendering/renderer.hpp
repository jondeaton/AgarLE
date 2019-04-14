#pragma once

#define GL_SILENCE_DEPRECATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <OpenGL/OpenGL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <exception>
#include <vector>
#include <string>
#include <math.h>
#include <optional>

#include "shader.hpp"
#include "engine/GameState.hpp"
#include <core/Entities.hpp>
#include <core/Player.hpp>

#define COLOR_LEN 3

#define PELLET_RADIUS 10
#define FOOD_RADIUS 50
#define VIRUS_RADIUS 100

#define DEFAULT_SCREEN_WIDTH 640
#define DEFAULT_SCREEN_HEIGHT 480
#define NUM_GRID_LINES 100

#define DEFAULT_ARENA_WIDTH 1000
#define DEFAULT_ARENA_HEIGHT 1000

#define WINDOW_NAME "AgarIO"

namespace agario {

  class Renderer {
  public:

    typedef Player<true> Player;
    typedef Cell<true> Cell;
    typedef Pellet<true> Pellet;
    typedef Food<true> Food;
    typedef Virus<true> Virus;

    std::shared_ptr<Player> player;
    GLFWwindow *window;

    explicit Renderer(agario::distance arena_width, agario::distance arena_height,
                      bool draw = true) : player(nullptr), window(nullptr),
                                          arena_width(arena_width), arena_height(arena_height),
                                          screen_width(DEFAULT_SCREEN_WIDTH), screen_height(DEFAULT_SCREEN_HEIGHT),
                                          shader(), _draw(draw),
                                          grid(arena_width, arena_height) {
      if (_draw)
        window = initialize_window();
      shader.generate_shader("../rendering/vertex.glsl", "../rendering/fragment.glsl");
      shader.use();
    }

    GLFWwindow *initialize_window() {
      // Initialize the library
      if (!glfwInit())
        throw std::exception();


      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

      // Create a windowed mode window and its OpenGL context
      window = glfwCreateWindow(screen_width, screen_height, WINDOW_NAME, nullptr, nullptr);

      if (window == nullptr) {
        glfwTerminate();
        std::cerr << "window create failed" << std::endl;
        throw std::exception();
      }

      glfwMakeContextCurrent(window);
      glewExperimental = GL_TRUE;

      GLenum err = glewInit();
      if (err != GLEW_OK)
        throw std::exception();

      return window;
    }

    float aspect_ratio() const {
      return (float) screen_width / (float) screen_height;
    }

    agario::Location to_target(float xpos, float ypos) {
      auto dx = 2 * (xpos - ((float) screen_width / 2)) / screen_width;
      auto dy = - 2 * (ypos - ((float) screen_height / 2)) / screen_height;
      return agario::Location(dx, dy);
    }

    void make_projections() {
      glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect_ratio(), 0.1f, 100.0f);
      shader.setMat4("projection_transform", projection);

      glm::mat4 view = glm::lookAt(
        glm::vec3(player->x(), player->y(), 3 * player->mass()), // Camera location in World Space
        glm::vec3(player->x(), player->y(), 0), // camera "looks at" location
        glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
      );
      shader.setMat4("view_transform", view);
    }

    void render_screen(agario::GameState<true> &state) {

      auto &players = state.players;
      auto &foods = state.foods;
      auto &pellets = state.pellets;
      auto &viruses = state.viruses;

      shader.use();
      make_projections();

      glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      grid.draw(shader);

      for (auto &food : foods)
        food.draw(shader);

      for (auto &virus : viruses)
        virus.draw(shader);

      for (auto &pellet : pellets)
        pellet.draw(shader);

      for (auto &plyr : players)
        plyr.draw(shader);

      player->draw(shader);

      glfwSwapBuffers(window);
      glfwPollEvents();
    }

    bool ready() {
      if (window == nullptr) return false;
      return !glfwWindowShouldClose(window);
    }

    void terminate() {
      glfwTerminate();
    }

  private:
    agario::distance arena_width;
    agario::distance arena_height;

    int screen_width;
    int screen_height;
    Shader shader;
    bool _draw;

    agario::Grid<NUM_GRID_LINES> grid;
  };
}