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
#include "renderables.hpp"

#define CIRCLE_SIDES 50
#define CIRCLE_VERTS (CIRCLE_SIDES + 2)
#define COLOR_LEN 3

#define PELLET_RADIUS 10
#define FOOD_RADIUS 50
#define VIRUS_RADIUS 100

#define DEFAULT_SCREEN_WIDTH 640
#define DEFAULT_SCREEN_HEIGHT 480

#define WINDOW_NAME "AgarIO"


// Defines circle to be rendered.
struct circle_obj {
  float verts[3 * CIRCLE_VERTS];
  float color[COLOR_LEN];
  unsigned int vao; // vertex attribute object
  unsigned int vbo; // vertex buffer object (gpu memory)
};

class AgarioRenderer {
public:

  explicit AgarioRenderer(bool draw=true) :
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

//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//#ifdef __APPLE__
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif

    // Create a windowed mode window and its OpenGL context
    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_NAME, nullptr, nullptr);

    if (window == nullptr) {
      glfwTerminate();
      std::cerr << "window create failed" << std::endl;
      throw std::exception();
    }

    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (err != GLEW_OK)
      throw std::exception();

    glViewport( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT ); // specifies the part of the window to which OpenGL will draw (in pixels), convert from normalised to pixels
    glMatrixMode( GL_PROJECTION ); // projection matrix defines the properties of the camera that views the objects in the world coordinate frame. Here you typically set the zoom factor, aspect ratio and the near and far clipping planes
    glLoadIdentity( ); // replace the current matrix with the identity matrix and starts us a fresh because matrix transforms such as glOrpho and glRotate cumulate, basically puts us at (0, 0, 0)
    glOrtho( 0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1 ); // essentially set coordinate system
    glMatrixMode( GL_MODELVIEW ); // (default matrix mode) modelview matrix defines how your objects are transformed (meaning translation, rotation and scaling) in your world
    glLoadIdentity( ); // same as above comment

    return window;
  }

  GLfloat lineVertices[6] = {
    200, 100, 0,
    100, 300, 0
  };

  void draw_grid() {
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_LINE_STIPPLE);
    glPushAttrib(GL_LINE_BIT);
    glLineWidth(10);
    glLineStipple(1, 0x00FF);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, lineVertices);
    glDrawArrays(GL_LINES, 0, 2);
    glDisableClientState(GL_VERTEX_ARRAY);
    glPopAttrib();
    glDisable(GL_LINE_STIPPLE);
    glDisable(GL_LINE_SMOOTH);
  }

  void draw_border() {
    GLfloat color[3] = { 1.0f, 0.0f, 0.0f };
    Circle circle;
    set_color(circle, color);
    draw_circle(0, 0, 100);
  }

  void draw_pellets(player &player, std::vector<pellet> &pellets) {
    for (auto &pellet : pellets)
      draw_circle(pellet.x - player.x + screen_width / 2,
                  pellet.y - player.y + screen_height / 2,
                  PELLET_RADIUS);
  }

  void draw_foods(player &player, std::vector<food> &foods) {
    for (auto &food : foods)
      draw_circle(food.x - player.x + screen_width / 2,
                  food.y - player.y + screen_height / 2,
                  FOOD_RADIUS);
  }

  void draw_viruses(player &player, std::vector<virus> &viruses) {
    for (auto &virus : viruses)
      draw_circle(virus.x - player.x + screen_width / 2,
                  virus.y - player.y + screen_height / 2,
                  VIRUS_RADIUS);
  }

  void draw_players(player &p, std::vector<player> &players) {
    for (auto &player : players)
      for (auto &cell : player.cells)
        draw_circle(cell.x - player.x + screen_width / 2,
                    cell.y - player.y + screen_height / 2,
                    cell.radius);
  }

  bool ready() {
    if (window == nullptr) return false;
    return !glfwWindowShouldClose(window);
  }

  void render_screen(player &p1, std::vector<player> &players,
    std::vector<food> &foods, std::vector<pellet> &pellets, std::vector<virus> &viruses) {

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear( GL_COLOR_BUFFER_BIT );

    draw_grid();
    draw_foods(p1, foods);
    draw_pellets(p1, pellets);
    draw_viruses(p1, viruses);

    draw_border();

    draw_players(p1, players);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  void terminate() {
    glfwTerminate();
  }

  void set_color(Circle &circle, GLfloat color[3]) {
    memcpy(circle.color, color, 3 * sizeof(GLfloat));
  }

private:
  GLfloat screen_width;
  GLfloat screen_height;
  GLFWwindow *window;
  Shader shader;
  bool draw;


  void draw_circle( GLfloat x, GLfloat y, GLfloat radius) {
    // todo: I think this is doing the same thing as the previous way?
    // except that with this we waste time re-allocating memory
    // todo: fix that...
    GLint numberOfSides = CIRCLE_VERTS;
    GLfloat z = 0;

    int numberOfVertices = numberOfSides + 2;

    GLfloat twicePi = 2.0f * M_PI;

    GLfloat circleVerticesX[numberOfVertices];
    GLfloat circleVerticesY[numberOfVertices];
    GLfloat circleVerticesZ[numberOfVertices];

    circleVerticesX[0] = x;
    circleVerticesY[0] = y;
    circleVerticesZ[0] = z;

    for (int i = 1; i < numberOfVertices; i++) {
      circleVerticesX[i] = x + ( radius * cos( i *  twicePi / numberOfSides ) );
      circleVerticesY[i] = y + ( radius * sin( i * twicePi / numberOfSides ) );
      circleVerticesZ[i] = z;
    }

    GLfloat allCircleVertices[( numberOfVertices ) * 3];

    for (int i = 0; i < numberOfVertices; i++) {
      allCircleVertices[i * 3] = circleVerticesX[i];
      allCircleVertices[( i * 3 ) + 1] = circleVerticesY[i];
      allCircleVertices[( i * 3 ) + 2] = circleVerticesZ[i];
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glColor3f(0.2, 0.5, 0.5);
    glVertexPointer(3, GL_FLOAT, 0, allCircleVertices);
    glDrawArrays(GL_TRIANGLE_FAN, 0, numberOfVertices);
    glDisableClientState(GL_VERTEX_ARRAY);
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
