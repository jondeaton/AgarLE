#pragma once

#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>

#include <exception>
#include <vector>
#include <string>
#include <math.h>

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

#define WINDOW_NAME "AgarIO"

namespace Agario {

  class Renderer {
  public:

    typedef Player<true> Player;
    typedef Cell<true> Cell;
    typedef Pellet<true> Pellet;
    typedef Food<true> Food;
    typedef Virus<true> Virus;

    explicit Renderer(bool draw=true) :
      screen_width(DEFAULT_SCREEN_WIDTH), screen_height(DEFAULT_SCREEN_HEIGHT),
      window(nullptr), shader(), draw(draw) {
      if (draw) {
        window = initialize_window();
        shader.generate_shader("../client/vertex.shader", "../client/fragment.shader");
        shader.use();
      }
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

    std::pair<GLfloat, GLfloat> to_screen_position(const Player &player, GLfloat x, GLfloat y) {
      auto screen_x = static_cast<GLfloat> ((x - player.x()) / screen_x_range);
      auto screen_y = static_cast<GLfloat> ((y - player.y()) / screen_y_range);
      return {screen_x, screen_y};
    }

    GLfloat lineVertices[6] = {
      0, 0, 0,
      0.5, 0.5, 0
    };

    void draw_grid() {

    }

    void draw_border() {
      GLfloat color[3] = {1.0f, 0.0f, 0.0f};
//    Circle circle;
//    set_color(circle, color);
      draw_circle(0, 0, 0.25);
    }

    void draw_pellets(Player &player, std::vector<Pellet> &pellets) {
      for (auto &pellet : pellets)
        draw_circle(pellet.x - player.x() + screen_width / 2,
                    pellet.y - player.y() + screen_height / 2,
                    PELLET_RADIUS);
    }

    void draw_foods(Player &player, std::vector<Food> &foods) {
      for (auto &food : foods)
        draw_circle(food.x - player.x() + screen_width / 2,
                    food.y - player.y() + screen_height / 2,
                    FOOD_RADIUS);
    }

    void draw_viruses(Player &player, std::vector<Virus> &viruses) {
      for (auto &virus : viruses)
        draw_circle(virus.x - player.x() + screen_width / 2,
                    virus.y - player.y() + screen_height / 2,
                    VIRUS_RADIUS);
    }

    void draw_players(Player &p, std::vector<Player> &players) {
      for (auto &player : players)
        for (auto &cell : player.cells)
          draw_circle(cell.x - player.x() + screen_width / 2,
                      cell.y - player.y() + screen_height / 2,
                      cell.radius());
    }

    void render_screen(Player &p1, std::vector<Player> &players,
                       std::vector<Food> &foods, std::vector<Pellet> &pellets, std::vector<Virus> &viruses) {

      glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      draw_grid();
      draw_foods(p1, foods);
      draw_pellets(p1, pellets);
      draw_viruses(p1, viruses);

      draw_border();

      draw_players(p1, players);

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
    GLfloat screen_width;
    GLfloat screen_height;
    GLFWwindow *window;
    Shader shader;
    bool draw;

    float screen_x_range;
    float screen_y_range;

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

    // todo: fix this!!!
//  void draw_line(lineVertices) {
//    glEnable(GL_LINE_SMOOTH);
//    glEnable(GL_LINE_STIPPLE);
//    glPushAttrib(GL_LINE_BIT);
//    glLineWidth(10);
//    glLineStipple(1, 0x00FF);
//    glEnableClientState(GL_VERTEX_ARRAY);
//    glVertexPointer(3, GL_FLOAT, 0, lineVertices);
//    glDrawArrays(GL_LINES, 0, 2);
//    glDisableClientState(GL_VERTEX_ARRAY);
//    glPopAttrib();
//    glDisable(GL_LINE_STIPPLE);
//    glDisable(GL_LINE_SMOOTH);
//  }


    void set_color(Circle &circle, GLfloat color[3]) {
      memcpy(circle.color, color, 3 * sizeof(GLfloat));
    }

//  void draw_circle(Circle &circle, GLfloat screen_x, GLfloat screen_y, GLfloat screen_radius) {
//    position_circle(circle, screen_x, screen_y, screen_radius);
//    render_circle(circle);
//  }
//
//  void render_circle(Circle &circle) {
//    shader.setVec3("col", circle.color[0], circle.color[1], circle.color[2]);
//    glBindVertexArray(circle.vao);
//    glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_VERTS);
//    glBindVertexArray(0);
//  }
//
//  void position_circle(Circle &circle, GLfloat x, GLfloat y, GLfloat radius) {
//    update_verts(circle, x, y, radius);
//
//    glGenVertexArrays(1, &circle.vao);
//    glGenBuffers(1, &circle.vbo);
//
//    glBindVertexArray(circle.vao);
//    glBindBuffer(GL_ARRAY_BUFFER, circle.vbo);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(circle.verts), circle.verts, GL_STREAM_DRAW);
//
//    // Position attribute
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
//    glEnableVertexAttribArray(0);
//  }
//
//  void update_verts(Circle &circle, GLfloat x, GLfloat y, GLfloat radius) {
//    circle.verts[0] = x / screen_width;
//    circle.verts[1] = y / screen_width;
//    circle.verts[2] = 0;
//
//    for (int i = 1; i < CIRCLE_VERTS; i++) {
//      circle.verts[i * 3] = static_cast<float> (x + (radius * cos(i *  2 * M_PI / CIRCLE_SIDES))) / screen_width;
//      circle.verts[i * 3 + 1] = static_cast<float> (y + (radius * sin(i * 2 * M_PI / CIRCLE_SIDES))) / screen_height;
//      circle.verts[i * 3 + 2] = 0;
//    }
//  }

  };


}