#pragma once

#include <exception>
#include <vector>
#include <string>
#include <math.h>

// todo: remove this
#define GL_SILENCE_DEPRECATION

#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>

#include "shader.h"
#include "entities.hpp"

#define CIRCLE_SIDES 50
#define CIRCLE_VERTS (CIRCLE_SIDES + 2)
#define COLOR_LEN 3

#define PELLET_RADIUS 10
#define FOOD_RADIUS 50
#define VIRUS_RADIUS 100

#define DEFAULT_SCREEN_WIDTH 800
#define DEFAULT_SCREEN_HEIGHT 600

#define WINDOW_NAME "AgarIO"


// Defines circle to be rendered.
class circle_obj {
  float verts[3 * CIRCLE_VERTS];
  float color[COLOR_LEN];
  unsigned int vao; // vertex attribute object
  unsigned int vbo; // vertex buffer object (gpu memory)
  circle_obj *next;
};

class AgarioRenderer {
public:

  AgarioRenderer() : 
  screen_width(DEFAULT_SCREEN_WIDTH), screen_height(DEFAULT_SCREEN_HEIGHT) {}
//  shader("client/vertex.shader", "client/fragment.shader") { }

  void initialize_window() {
    GLFWwindow *window;

    // Initialize the library
    if (!glfwInit())
      throw std::exception();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_NAME, nullptr, nullptr);

    if (!window) {
      glfwTerminate();
      std::cerr << "window create failed" << std::endl;
      throw std::exception();
    }

    glfwMakeContextCurrent(window);
  }

  // todo: how to draw lines in OpenGL?
  void draw_grid() { }
  void draw_border() { }

  void draw_pellets(player &player, std::vector<pellet> &pellets) {
    for (auto &pellet : pellets)
      draw_circle(pellet.circle,
                  pellet.x - player.x + screen_width / 2,
                  pellet.y - player.y + screen_height / 2,
                  PELLET_RADIUS);
  }

  void draw_foods(player &player, std::vector<food> &foods) {
    for (auto &food : foods)
      draw_circle(food.circle,
                  food.x - player.x + screen_width / 2,
                  food.y - player.y + screen_height / 2,
                  FOOD_RADIUS);
  }

  void draw_viruses(player &player, std::vector<virus> &viruses) {
    for (auto &virus : viruses)
      draw_circle(virus.circle,
                  virus.x - player.x + screen_width / 2,
                  virus.y - player.y + screen_height / 2,
                  VIRUS_RADIUS);
  }

  void draw_players(player &p, std::vector<player> &players) {
    for (auto &player : players)
      for (auto &cell : player.cells)
        draw_circle(cell.circle,
                    cell.x - player.x + screen_width / 2,
                    cell.y - player.y + screen_height / 2,
                    cell.radius);
  }


private:
  GLfloat screen_width;
  GLfloat screen_height;
//  Shader shader;

  void draw_circle(circle &circle, GLfloat screen_x, GLfloat screen_y, GLfloat screen_radius) {
    position_circle(circle, screen_x, screen_y, screen_radius);
    render_circle(circle);
  }

  void render_circle(circle &circle) {
//    shader.setVec3("col", circle.color[0], circle.color[1], circle.color[2]);
    glBindVertexArray(circle.vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_VERTS);
    glBindVertexArray(0);
  }

  void position_circle(circle &circle, GLfloat x, GLfloat y, GLfloat radius) {
    update_verts(circle, x, y, radius);

    glGenVertexArrays(1, &circle.vao);
    glGenBuffers(1, &circle.vbo);

    glBindVertexArray(circle.vao);
    glBindBuffer(GL_ARRAY_BUFFER, circle.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle.verts), circle.verts, GL_STREAM_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
  }

  void update_verts(circle &circle, GLfloat x, GLfloat y, GLfloat radius) {
    circle.verts[0] = x / screen_width;
    circle.verts[1] = y / screen_width;
    circle.verts[2] = 0;

    for (int i = 1; i < CIRCLE_VERTS; i++) {
      circle.verts[i * 3] = static_cast<float> (x + (radius * cos(i *  2 * M_PI / CIRCLE_SIDES))) / screen_width;
      circle.verts[i * 3 + 1] = static_cast<float> (y + (radius * sin(i * 2 * M_PI / CIRCLE_SIDES))) / screen_width;
      circle.verts[i * 3 + 2] = 0;
    }
  }

};
