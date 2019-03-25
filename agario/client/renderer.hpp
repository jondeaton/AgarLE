#pragma once

#include <exception>
#include <vector>
#include <string>

#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>

#include "shader.h"

#define CIRCLE_SIDES 50
#define CIRCLE_VERTS (CIRCLE_SIDES + 2)
#define COLOR_LEN 3
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

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

  void draw_grid() {

  }

  void draw_foods() {

  }

  void draw_pellets() {

  }

  void draw_viruses() {

  }

  void draw_border() {

  }

  void draw_players() {

  }

private:

};
