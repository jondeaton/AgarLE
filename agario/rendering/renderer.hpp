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
#include <core/Entities.hpp>
#include <core/Player.hpp>

#define CIRCLE_SIDES 50
#define CIRCLE_VERTS (CIRCLE_SIDES + 2)
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

namespace Agario {

  class Renderer {
  public:

    typedef Player<true> Player;
    typedef Cell<true> Cell;
    typedef Pellet<true> Pellet;
    typedef Food<true> Food;
    typedef Virus<true> Virus;

    std::shared_ptr<Player> player;

    explicit Renderer(Agario::distance arena_width, Agario::distance arena_height,
                      bool draw = true) : player(nullptr),
                                          arena_width(arena_width), arena_height(arena_height),
                                          screen_width(DEFAULT_SCREEN_WIDTH), screen_height(DEFAULT_SCREEN_HEIGHT),
                                          window(nullptr), shader(), draw(draw),
                                          grid(arena_width, arena_height){
      if (draw)
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

    void draw_border() {

    }

    float aspect_ratio() const {
      return (float) screen_width / (float) screen_height;
    }

    void make_projections() {
      glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect_ratio(), 0.1f, 100.0f);
      GLint proj_location = glGetUniformLocation(shader.program, "projection_transform");
      glUniformMatrix4fv(proj_location, 1, GL_FALSE, &projection[0][0]);

      glm::mat4 view = glm::lookAt(
        glm::vec3(player->x(), player->y(), player->mass()), // Camera location in World Space
        glm::vec3(player->x(), player->y(), 0), // camera "looks at" location
        glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
      );
      GLint view_location = glGetUniformLocation(shader.program, "view_transform");
      glUniformMatrix4fv(view_location, 1, GL_FALSE, &view[0][0]);
    }

    void render_screen(std::vector<Player> &players, std::vector<Food> &foods,
                       std::vector<Pellet> &pellets, std::vector<Virus> &viruses) {

      shader.use();
      make_projections();

      glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      make_projections();

      grid.draw(shader);

      for (auto &food : foods)
        food.draw(shader);

      for (auto &virus : viruses)
        virus.draw(shader);

      for (auto &pellet : pellets)
        pellet.draw(shader);

      draw_border();

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
    int screen_width;
    int screen_height;
    GLFWwindow *window;
    Shader shader;
    bool draw;

    Agario::distance arena_width;
    Agario::distance arena_height;

    Grid<NUM_GRID_LINES> grid;

    template<unsigned NSides>
    void set_color(Circle<NSides> &circle, GLfloat color[3]) {
      memcpy(circle.color, color, 3 * sizeof(GLfloat));
    }
  };
}