#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <rendering/shader.hpp>

static const GLuint WIDTH = 800;
static const GLuint HEIGHT = 600;

/* ourColor is passed on to the fragment shader. */
#define CIRCLE_SIDES 100
#define CIRCLE_VERTS (CIRCLE_SIDES + 2)
#define COLOR_LEN 3

typedef float distance;

class Circle {
public:

  Circle(float x, float y) : _x(x), _y(y), _radius(1) {
    update_verts();

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STREAM_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
  }

  float verts[3 * CIRCLE_VERTS];
  float color[COLOR_LEN] = {0.5, 0.5, 0.5};
  unsigned int vao; // vertex attribute object
  unsigned int vbo; // vertex buffer object (gpu memory)

  float x() const {
    return _x;
  }

  float y() const {
    return _y;
  }

  void set_location(float x, float y) {
    _x = x;
    _y = y;
  }

  void set_radius(float radius) {
    _radius = radius;
  }

  void draw(Shader &shader) {
    shader.setVec3("color", color[0], color[1], color[2]);

    auto world_position = glm::vec3(_x, _y, 0);

    glm::mat4 position_transform(1);
    glm::mat4 scale_transform(1);
    position_transform = glm::translate(position_transform, world_position);
    scale_transform = glm::scale(scale_transform, glm::vec3(_radius, _radius, 0));

    glm::mat4 model_matrix = position_transform * scale_transform;

    GLint model_loc = glGetUniformLocation(shader.program, "model_transform");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, &model_matrix[0][0]);

    // draw them!
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_VERTS);
    glBindVertexArray(0);
  }

private:
  float _x;
  float _y;
  float _radius;

  void update_verts() {
    verts[0] = 0;
    verts[1] = 0;
    verts[2] = 0;
    for (int i = 1; i < CIRCLE_VERTS; i++) {
      verts[i * 3] = static_cast<float> (0 + (1 * cos(i *  2 * M_PI / CIRCLE_SIDES)));
      verts[i * 3 + 1] = static_cast<float> (0 + (1 * sin(i * 2 * M_PI / CIRCLE_SIDES)));
      verts[i * 3 + 2] = 0;
    }
  }
};

template <unsigned NLines>
class Grid {
public:
  Grid(distance arena_width, distance arena_height) :
    arena_width(arena_width), arena_height(arena_height),
    vao_vertical(0), vao_horizontal(0) {
    _create_vertices();
  }

  void draw(Shader &shader) {
    int num_vertices = 2 * NLines;

    shader.setVec3("color", color[0], color[1], color[2]);

    glm::mat4 model_matrix(1);
    model_matrix = glm::scale(model_matrix, glm::vec3(arena_width, arena_height, 0));

    GLint model_loc = glGetUniformLocation(shader.program, "model_transform");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, &model_matrix[0][0]);


    glGenBuffers(1, &vao_vertical);
    glBindBuffer(GL_ARRAY_BUFFER, vao_vertical);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertical_verts), vertical_verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    _draw_verts(vao_vertical, num_vertices);

    glGenBuffers(1, &vao_horizontal);
    glBindBuffer(GL_ARRAY_BUFFER, vao_horizontal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(horizontal_verts), horizontal_verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    _draw_verts(vao_horizontal, num_vertices);
  }

private:
  distance arena_width;
  distance arena_height;

  float color[COLOR_LEN];

  GLuint vao_vertical;
  GLfloat vertical_verts[2 * 3 * NLines];

  GLuint vao_horizontal;
  GLfloat horizontal_verts[2 * 3 * NLines];

  void _draw_verts(GLuint vao, int num_vertices) {
    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, num_vertices);
//    glBindVertexArray(0);
  }

  void _create_vertices() {
    _create_vertical_verts();
    _create_horiz_verts();
  }

  void _create_vertical_verts() {
    // vertical lines
    for (unsigned i = 0; i < NLines; i++) {
      vertical_verts[6 * i] = static_cast<GLfloat>(i) / NLines;
      vertical_verts[6 * i + 1] = 0;
      vertical_verts[6 * i + 2] = 0;

      vertical_verts[6 * i + 3] = static_cast<GLfloat>(i) / NLines;
      vertical_verts[6 * i + 4] = 1;
      vertical_verts[6 * i + 5] = 0;
    }

  }

  void _create_horiz_verts() {
    // horizontal lines
    for (unsigned i = 0; i < NLines; i++) {
      horizontal_verts[6 * i] = 0;
      horizontal_verts[6 * i + 1] = static_cast<GLfloat>(i) / NLines;
      horizontal_verts[6 * i + 2] = 0;

      horizontal_verts[6 * i + 3] = 1;
      horizontal_verts[6 * i + 4] = static_cast<GLfloat>(i) / NLines;
      horizontal_verts[6 * i + 5] = 0;
    }

  }
};

void set_view_projection(Shader &shader, float x, float y, float camera_z) {
  glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);
  GLint proj_location = glGetUniformLocation(shader.program, "projection_transform");
  glUniformMatrix4fv(proj_location, 1, GL_FALSE, &Projection[0][0]);

  glm::mat4 View = glm::lookAt(
    glm::vec3(x, y, camera_z), // Camera location in World Space
    glm::vec3(x, y, 0), // camera "looks at" location
    glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
  );
  GLint view_location = glGetUniformLocation(shader.program, "view_transform");
  glUniformMatrix4fv(view_location, 1, GL_FALSE, &View[0][0]);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
  (void) mods;
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    double xpos, ypos;
    //getting cursor position
    glfwGetCursorPos(window, &xpos, &ypos);
    std::cout << "Cursor Position at (" << xpos << " : " << ypos << std::endl;
  }
}

int main(int argc, char *argv[]) {
  (void) argc;
  (void) argv;

  GLuint vao;
  GLFWwindow* window;
  double time;

  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

  window = glfwCreateWindow(WIDTH, HEIGHT, __FILE__, nullptr, nullptr);
  glfwMakeContextCurrent(window);
  glewExperimental = GL_TRUE;
  glewInit();

  glfwSetMouseButtonCallback(window, mouse_button_callback);

//  glMatrixMode(GL_PROJECTION);
//  glLoadIdentity();
//  glOrtho(0.0f, WIDTH, HEIGHT, 0.0f, 0.0f, 1.0f);

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glViewport(0, 0, WIDTH, HEIGHT);

  Shader shader("../rendering/vertex.glsl", "../rendering/fragment.glsl");

  Circle circle(0, 0);
  Circle c(0, 0);
  Circle origin(0, 0);

  Grid<100> grid(100, 100);
  GLuint vbo;

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    double xpos, ypos;
    //getting cursor position
    glfwGetCursorPos(window, &xpos, &ypos);

    glClear(GL_COLOR_BUFFER_BIT);

    shader.use();

    time = glfwGetTime();

    set_view_projection(shader, circle.x(), circle.y(), 1 + 2 * time);

    circle.set_location(2 - time / 3, 2);
    circle.set_radius(0.1 * (1 + 0.5 * sin(time)));
//    circle.draw(shader);

    c.set_location(0.5, 0.5);
    c.set_radius(0.1 * (1 + 0.5 * cos(time)));
//    c.draw(shader);

    origin.set_radius(0.1);
//    origin.draw(shader);
    
    grid.draw(shader);

    glfwSwapBuffers(window);
  }
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glfwTerminate();
  return EXIT_SUCCESS;
}