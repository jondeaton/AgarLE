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

  class Grid {
  public:
    Grid(Agario::distance arena_width, Agario::distance arena_height) :
    arena_width(arena_width), arena_height(arena_height),
    vbo_vertical(0), vbo_horizontal(0) {
      _create_vertices();
    }

    void draw() {
      glEnableVertexAttribArray(attribute_v_coord);
      // Describe our vertices array to OpenGL (it can't guess its format automatically)
      glBindBuffer(GL_ARRAY_BUFFER, vbo_sprite_vertices);
      glVertexAttribPointer(
        attribute_v_coord, // attribute
        4,                 // number of elements per vertex, here (x,y,z)
        GL_FLOAT,          // the type of each element
        GL_FALSE,          // take our values as-is
        0,                 // no extra data between each position
        0                  // offset of first element
      );
    }

  private:
    Agario::distance arena_width;
    Agario::distance arena_height;

    GLuint vbo_vertical;
    GLuint vbo_horizontal;

    void _create_vertical_verts() {
      int num_vert_lines = static_cast<int>(arena_width / GRID_SPACING);

      // allocate vector of vertices
      auto verts = new GLfloat[2 * 3 * num_vert_lines];

      // vertical lines
      for (int i = 0; i < num_vert_lines; i++) {
        verts[6 * i] = i * GRID_SPACING;
        verts[6 * i + 1] = 0;
        verts[6 * i + 2] = 0;

        verts[6 * i + 3] = i * GRID_SPACING;
        verts[6 * i + 4] = static_cast<GLfloat>(arena_height);
        verts[6 * i + 5] = 0;
      }

      glGenBuffers(1, &vbo_vertical);
      glBindBuffer(GL_ARRAY_BUFFER, vbo_vertical);
      glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

      delete[] verts;
    }

    void _create_horiz_verts() {
      int num_horz_lines = static_cast<int>(arena_height / GRID_SPACING);
      auto verts = new GLfloat[2 * 3 * num_horz_lines];

      // horizontal lines
      for (int i = 0; i < num_horz_lines; i++) {
        verts[6 * i] = 0;
        verts[6 * i + 1] = i * GRID_SPACING;
        verts[6 * i + 2] = 0;

        verts[6 * i + 3] = static_cast<GLfloat>(arena_width);
        verts[6 * i + 4] = i * GRID_SPACING;
        verts[6 * i + 5] = 0;
      }

      glGenBuffers(1, &vbo_horizontal);
      glBindBuffer(GL_ARRAY_BUFFER, vbo_horizontal);
      glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

      delete[] verts;
    }

    void _create_vertices() {
      _create_vertical_verts();
      _create_horiz_verts();
    }

  };


  class Renderer {
  public:

    typedef Player<true> Player;
    typedef Cell<true> Cell;
    typedef Pellet<true> Pellet;
    typedef Food<true> Food;
    typedef Virus<true> Virus;

    std::shared_ptr<Player> player;

    explicit Renderer(Agario::distance arena_width, Agario::distance arena_height,
      bool draw=true) : player(nullptr),
      arena_width(arena_width), arena_height(arena_height),
      screen_width(DEFAULT_SCREEN_WIDTH), screen_height(DEFAULT_SCREEN_HEIGHT),
      window(nullptr), shader(), draw(draw) {
      if (draw) {
        window = initialize_window();
        shader.generate_shader("../rendering/vertex.shader", "../rendering/fragment.shader");
        shader.use();
      }
      create_grid_vertices();

      glm::mat4 projection = glm::ortho(0, screen_width, screen_height, 0);
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

    void create_grid_vertices() {

    }

    GLfloat to_screen_x(Agario::distance x) const {
      return static_cast<GLfloat> (2 * (x - player->x()) / view_width);
    }

    GLfloat to_screen_y(Agario::distance y) const {
      return static_cast<GLfloat> (2 * (y - player->y()) / view_height);
    }

    Agario::distance view_bottom() const {
      return player->y() - view_height / 2;
    }

    Agario::distance view_top() const {
      return player->y() + view_height / 2;
    }

    Agario::distance view_left() const {
      return player->x() - view_width / 2;
    }

    Agario::distance view_right() const {
      return player->x() + view_width / 2;
    }

    GLfloat to_screen_size(Agario::distance d) const {
      // todo: yeah this isn't right
      return static_cast<GLfloat>(d / view_width);
    }

    void draw_grid() {

      auto scale = player->mass();

      glm::mat4 projection = glm::ortho(0.0f,
        1.0f * screen_width * scale,
        1.0f * screen_height * scale,
        0.0f);
    }

    void draw_border() {
      // todo
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

    void draw_player(const Player &p) {
      for (auto &cell : p.cells)
        draw_circle(to_screen_x(cell.x), to_screen_y(cell.y),
                    to_screen_size(cell.radius()));
    }


    void make_projections() {
//      glm::mat4 Projection = glm::perspective(glm::radians(45.0f), screen_width / screen_height, 0.1f, 100.0f);
      // Or, for an ortho camera :
      glm::mat4 Projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f); // In world coordinates

      auto x = player->x();
      auto y = player->y();

      // Camera matrix
      glm::mat4 View = glm::lookAt(
        glm::vec3(x, y, player->mass()), // Camera location in World Space
        glm::vec3(x, y, 0), // and looks at the origin
        glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
      );

      // Model matrix : an identity matrix (model will be at the origin)
      glm::mat4 Model = glm::mat4(1.0f);

      // Our ModelViewProjection : multiplication of our 3 matrices
      glm::mat4 mvp = Projection * View * Model;

      // Get a handle for our "MVP" uniform
      // Only during the initialisation
      GLuint MatrixID = glGetUniformLocation(shader.program, "MVP");

      // Send our transformation to the currently bound shader, in the "MVP" uniform
      // This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
      glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
    }

    void render_screen(Player &p1, std::vector<Player> &players,
                       std::vector<Food> &foods, std::vector<Pellet> &pellets, std::vector<Virus> &viruses) {

      make_projections();

      glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      draw_grid();
      draw_foods(p1, foods);
      draw_pellets(p1, pellets);
      draw_viruses(p1, viruses);

      draw_border();

      for (auto &other_player : players)
        draw_player(other_player);

      draw_player(*player);

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

    // todo: make these scale with player mass
    Agario::distance view_width = 100;
    Agario::distance view_height = 100;

    Agario::distance arena_width;
    Agario::distance arena_height;

    void set_color(Circle &circle, GLfloat color[3]) {
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

  void draw_circle(Circle &circle, GLfloat screen_x, GLfloat screen_y, GLfloat screen_radius) {
    position_circle(circle, screen_x, screen_y, screen_radius);
    render_circle(circle);
  }

  void render_circle(Circle &circle) {
    shader.setVec3("col", circle.color[0], circle.color[1], circle.color[2]);
    glBindVertexArray(circle.vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_VERTS);
    glBindVertexArray(0);
  }

  void position_circle(Circle &circle, GLfloat x, GLfloat y, GLfloat radius) {
    update_verts(circle, x, y, radius);

    glGenVertexArrays(1, &circle.vao);
    glGenBuffers(1, &circle.vbo);

    glBindVertexArray(circle.vao);
    glBindBuffer(GL_ARRAY_BUFFER, circle.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle.verts), circle.verts, GL_STREAM_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
  }

  void update_verts(Circle &circle, GLfloat x, GLfloat y, GLfloat radius) {
    circle.verts[0] = x / screen_width;
    circle.verts[1] = y / screen_width;
    circle.verts[2] = 0;

    for (int i = 1; i < CIRCLE_VERTS; i++) {
      circle.verts[i * 3] = static_cast<float> (x + (radius * cos(i *  2 * M_PI / CIRCLE_SIDES))) / screen_width;
      circle.verts[i * 3 + 1] = static_cast<float> (y + (radius * sin(i * 2 * M_PI / CIRCLE_SIDES))) / screen_height;
      circle.verts[i * 3 + 2] = 0;
    }
  }

    int round_up(int n, int multiple) {
      if (multiple == 0)
        return n;

      int remainder = abs(n) % multiple;
      if (remainder == 0)
        return n;

      if (n < 0)
        return -(abs(n) - remainder);
      else
        return n + multiple - remainder;
    }

  };
}