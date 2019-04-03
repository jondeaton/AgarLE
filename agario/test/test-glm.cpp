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

class Circle {
public:

  Circle(float x, float y) : _x(x), _y(y), _radius(0.1) {
    update_verts();

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STREAM_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
  }

  float verts[3 * CIRCLE_VERTS];
  float color[COLOR_LEN] = {0.5, 0.5, 0.5};
  unsigned int vao; // vertex attribute object
  unsigned int vbo; // vertex buffer object (gpu memory)

  void set_location(float x, float y) {
    _x = x;
    _y = y;
  }

  void set_radius(float radius) {
    _radius = radius;
//    update_verts();
  }

  void draw(Shader &shader) {
    auto world_position = glm::vec3(_x, _y, 0);
    glm::mat4 position_transform(1);
    position_transform = glm::translate(position_transform, world_position);

    GLint transform_location = glGetUniformLocation(shader.program, "position_translation");
    glUniformMatrix4fv(transform_location, 1, GL_FALSE, &position_transform[0][0]);

    glm::mat4 scale_transform(1);
    scale_transform = glm::scale(scale_transform, glm::vec3(_radius, _radius, 0));
    GLint scale_location = glGetUniformLocation(shader.program, "scale_transform");
    glUniformMatrix4fv(scale_location, 1, GL_FALSE, &scale_transform[0][0]);

    shader.setVec3("ourColor", color[0], color[1], color[2]);
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
      verts[i * 3] = static_cast<float> (0 + (_radius * cos(i *  2 * M_PI / CIRCLE_SIDES)));
      verts[i * 3 + 1] = static_cast<float> (0 + (_radius * sin(i * 2 * M_PI / CIRCLE_SIDES)));
      verts[i * 3 + 2] = 0;
    }
  }
};


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    double xpos, ypos;
    //getting cursor position
    glfwGetCursorPos(window, &xpos, &ypos);
    std::cout << "Cursor Position at (" << xpos << " : " << ypos << std::endl;
  }
}

int main(int argc, char *argv[]) {
  GLint transform_location;
  GLuint vbo;
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

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0f, WIDTH, HEIGHT, 0.0f, 0.0f, 1.0f);

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glViewport(0, 0, WIDTH, HEIGHT);

  Shader shader("../rendering/vertex.shader", "../rendering/fragment.shader");

  Circle circle(0.5f, 0.5f);

  static GLfloat vertices[] = {
    /*   Positions          Colors */
    0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
  };

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  /* Position attribute */
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
  glEnableVertexAttribArray(0);

  /* Color attribute */
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);
  glBindVertexArray(0);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    double xpos, ypos;
    //getting cursor position
    glfwGetCursorPos(window, &xpos, &ypos);

    glClear(GL_COLOR_BUFFER_BIT);

    shader.use();

    float x = 2 * (xpos - WIDTH / 2) / WIDTH;
    float y = - 2 * (ypos - HEIGHT / 2) / HEIGHT;

    time = glfwGetTime();

    circle.set_location(x, y);
    circle.set_radius(2 * (1 + 0.5 * sin(time)));
    circle.draw(shader);

//    transform_location = glGetUniformLocation(shader.program, "transform");
//    GLfloat transform[] = {
//      0.0f, 0.0f, 0.0f, 0.0f,
//      0.0f, 0.0f, 0.0f, 0.0f,
//      0.0f, 0.0f, 1.0f, 0.0f,
//      0.0f, 0.0f, 0.0f, 1.0f,
//    };

//    transform[0] = 1.0f * sin(3.14 * time);
//    transform[5] = 1.0f * cos(time);
//    glUniformMatrix4fv(transform_location, 1, GL_FALSE, transform);

//    glBindVertexArray(vao);
//    glDrawArrays(GL_TRIANGLES, 0, 3);
//    glBindVertexArray(0);

    glfwSwapBuffers(window);
  }
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glfwTerminate();
  return EXIT_SUCCESS;
}