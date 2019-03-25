#pragma once

#include <vector>
#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>

#include <engine/Entities.hpp>
#include <engine/Player.hpp>

#define CIRCLE_SIDES 50
#define CIRCLE_VERTS (CIRCLE_SIDES + 2)
#define COLOR_LEN 3
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600


struct Circle {
  float verts[3 * CIRCLE_VERTS];
  float color[COLOR_LEN];
  unsigned int vao; // vertex attribute object
  unsigned int vbo; // vertex buffer object (gpu memory)
};


struct RenderableCell : public Agario::Cell {
  GLfloat x;
  GLfloat y;
  GLfloat radius;
  Circle circle;
};

struct player {
  GLfloat x;
  GLfloat y;
  GLfloat color[3];
  std::vector<RenderableCell> cells;
};

struct food {
  GLfloat x;
  GLfloat y;
  Circle circle;
};

struct pellet {
  GLfloat x;
  GLfloat y;
  Circle circle;
};

struct virus {
  GLfloat x;
  GLfloat y;
  Circle circle;
};