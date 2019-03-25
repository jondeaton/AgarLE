#pragma once

#include <vector>
#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>


#define CIRCLE_SIDES 50
#define CIRCLE_VERTS (CIRCLE_SIDES + 2)
#define COLOR_LEN 3
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

class Circle {


};

struct circle {
  float verts[3 * CIRCLE_VERTS];
  float color[COLOR_LEN];
  unsigned int vao; // vertex attribute object
  unsigned int vbo; // vertex buffer object (gpu memory)
};

struct cell {
  GLfloat x;
  GLfloat y;
  GLfloat radius;
  circle circle;
};

struct player {
  GLfloat x;
  GLfloat y;
  GLfloat color[3];
  std::vector<cell> cells;
};

struct food {
  GLfloat x;
  GLfloat y;
  circle circle;
};