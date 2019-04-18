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
#define NUM_GRID_LINES 11

#define WINDOW_NAME "AgarIO"

namespace agario {

  class Renderer {
  public:

    typedef Player<true> Player;
    typedef Cell<true> Cell;
    typedef Pellet<true> Pellet;
    typedef Food<true> Food;
    typedef Virus<true> Virus;

    GLFWwindow *window;

    explicit Renderer(agario::distance arena_width, agario::distance arena_height,
                      bool draw = true) : window(nullptr),
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

    template<typename T>
    inline glm::tmat4x4<T, glm::defaultp> inv_perspective(T const &fovy, T const &aspect,
                                                          T const &zNear, T const &zFar) {
      // from https://gist.github.com/tristanpenman/c45dcc5ef959e0d1f33a
      glm::tmat4x4<T, glm::defaultp> m(static_cast<T>(0));

      const T tanHalfFovy = tan(fovy / static_cast<T>(2));
      m[0][0] = tanHalfFovy * aspect;
      m[1][1] = tanHalfFovy * aspect;
      m[3][2] = static_cast<T>(-1);

      const T d = static_cast<T>(2) * zFar * zNear;
      m[2][3] = (zNear - zFar) / d;
      m[3][3] = (zFar + zNear) / d;

      return m;
    }

    /// converts a screen position to a world position
    /// \param xpos screen horizontal position
    /// \param ypos screen vertical position
    /// \return world location
    agario::Location to_target(Player &player, float xpos, float ypos) {

      // normalized device coordinates (from -1 to 1)
      auto ndc_x = 2 * (xpos / screen_width) - 1;
      auto ndc_y = 1 - 2 * (ypos / screen_height);
      auto loc = glm::vec4(ndc_x, ndc_y, 1.0, 1);

      auto perspective = perspective_projection(player);
      auto view = view_projection(player);

      auto world_loc = glm::inverse(perspective * view) * loc;
      auto w = world_loc[3];
      auto x = world_loc[0] / w;
      auto y = world_loc[1] / w;

      return agario::Location(x, y);
    }

    void make_projections(const Player &player) {
      shader.setMat4("projection_transform", perspective_projection(player));
      shader.setMat4("view_transform", view_projection(player));
    }

    GLfloat camera_z(const Player &player) {
      return 3 * player.mass();
    }

    glm::mat4 perspective_projection(const Player &player) {
      return glm::perspective(glm::radians(45.0f), aspect_ratio(), 0.1f, camera_z(player));
    }

    glm::mat4 view_projection(const Player &player) {
      return glm::lookAt(
        glm::vec3(player.x(), player.y(), camera_z(player)), // Camera location in World Space
        glm::vec3(player.x(), player.y(), 0), // camera "looks at" location
        glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
      );
    }

    void render_screen(Player &player, agario::GameState<true> &state) {
      shader.use();

      make_projections(player);

      glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      grid.draw(shader);

      for (auto &food : state.foods)
        food.draw(shader);

      for (auto &virus : state.viruses)
        virus.draw(shader);

      for (auto &pellet : state.pellets)
        pellet.draw(shader);

      for (auto &pair : state.players)
        pair.second.draw(shader);

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