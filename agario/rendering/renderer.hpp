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
#define GRID_SPACING 25

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

      // Create a windowed mode window and its OpenGL context
      GLFWwindow *window = glfwCreateWindow(screen_width, screen_height, WINDOW_NAME, nullptr, nullptr);

      if (window == nullptr) {
        glfwTerminate();
        std::cerr << "window create failed" << std::endl;
        throw std::exception();
      }

      glfwMakeContextCurrent(window);

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
      glm::mat4 Projection = glm::perspective(glm::radians(45.0f), aspect_ratio(), 0.1f, 100.0f);
      GLint proj_location = glGetUniformLocation(shader.program, "projection_transform");
      glUniformMatrix4fv(proj_location, 1, GL_FALSE, &Projection[0][0]);

      glm::mat4 View = glm::lookAt(
        glm::vec3(player->x(), player->y(), 3), // Camera location in World Space
        glm::vec3(player->x(), player->y(), 0), // camera "looks at" location
        glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
      );
      GLint view_location = glGetUniformLocation(shader.program, "view_transform");
      glUniformMatrix4fv(view_location, 1, GL_FALSE, &View[0][0]);
    }

    void render_screen(std::vector<Player> &players, std::vector<Food> &foods,
                       std::vector<Pellet> &pellets, std::vector<Virus> &viruses) {

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

    Grid<GRID_SPACING> grid;

    template<unsigned NSides>
    void set_color(Circle<NSides> &circle, GLfloat color[3]) {
      memcpy(circle.color, color, 3 * sizeof(GLfloat));
    }

    void draw_circle(GLfloat x, GLfloat y, GLfloat radius) {
      // todo: I think this is doing the same thing as the previous way?
      // except that with this we waste time re-allocating memory
      // todo: fix that...
      GLint numberOfSides = CIRCLE_VERTS;
      GLfloat z = 0;

      int numberOfVertices = numberOfSides + 2;

      GLfloat circleVerticesX[numberOfVertices];
      GLfloat circleVerticesY[numberOfVertices];
      GLfloat circleVerticesZ[numberOfVertices];

      circleVerticesX[0] = x;
      circleVerticesY[0] = y;
      circleVerticesZ[0] = z;

      for (int i = 1; i < numberOfVertices; i++) {
        circleVerticesX[i] = x + (radius * cos(i * 2.0f * M_PI / numberOfSides));
        circleVerticesY[i] = y + (radius * sin(i * 2.0f * M_PI / numberOfSides));
        circleVerticesZ[i] = z;
      }

      GLfloat allCircleVertices[3 * numberOfVertices];

      for (int i = 0; i < numberOfVertices; i++) {
        allCircleVertices[i * 3] = circleVerticesX[i];
        allCircleVertices[(i * 3) + 1] = circleVerticesY[i];
        allCircleVertices[(i * 3) + 2] = circleVerticesZ[i];
      }

      glEnableClientState(GL_VERTEX_ARRAY);
      glColor3f(0.2, 0.5, 0.5);
      glVertexPointer(3, GL_FLOAT, 0, allCircleVertices);
      glDrawArrays(GL_TRIANGLE_FAN, 0, numberOfVertices);
      glDisableClientState(GL_VERTEX_ARRAY);
    }

    void draw_line(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) {
      GLfloat line_vertices[6] = {
        x1, y1, 0,
        x2, y2, 0
      };
      draw_line_vertices(line_vertices);
    }

    void draw_line_vertices(GLfloat *line_vertices) {
      glEnable(GL_LINE_SMOOTH);
      glPushAttrib(GL_LINE_BIT);
      glColor3f(0.9, 0.9, 0.9);
      glLineWidth(1);
      glLineStipple(1, 0x00FF);
      glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3, GL_FLOAT, 0, line_vertices);
      glDrawArrays(GL_LINES, 0, 2);
      glDisableClientState(GL_VERTEX_ARRAY);
      glPopAttrib();
      glDisable(GL_LINE_SMOOTH);
    }
  };
}