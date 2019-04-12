#pragma once

#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <core/Ball.hpp>
#include <rendering/shader.hpp>

#define COLOR_LEN 3

namespace agario {

  template<unsigned NSides>
  struct Circle {
    float verts[3 * (NSides + 2)];
    float color[COLOR_LEN];
    unsigned int vao; // vertex attribute object
    unsigned int vbo; // vertex buffer object (gpu memory)
  };

  template<unsigned NSides>
  class RenderableBall : virtual public Ball {
  public:
    using Ball::Ball;

    void draw(Shader &shader) {
      if (!_initialized) _initialize();

      shader.setVec4("color", circle.color[0], circle.color[1], circle.color[2], 1.0);

      // world location
      auto location = glm::vec3(x, y, 0);
      glm::mat4 position_transform(1);
      position_transform = glm::translate(position_transform, location);

      // scaling
      glm::mat4 scale_transform(1);
      scale_transform = glm::scale(scale_transform, glm::vec3(radius(), radius(), 0));

      shader.setMat4("model_transform", position_transform * scale_transform);

      // draw them!
      glBindVertexArray(circle.vao);
      glDrawArrays(GL_TRIANGLE_FAN, 0, NVertices);
      glBindVertexArray(0);
    }

    ~RenderableBall() {
      if (_initialized) {
        glDeleteVertexArrays(1, &circle.vao);
        glDeleteBuffers(1, &circle.vbo);
      }
    }

  private:
    static constexpr unsigned NVertices = NSides + 2;
    Circle<NSides> circle;
    bool _initialized = false;

    void _initialize() {
      _create_vertices();

      circle.color[0] = 1.0;
      circle.color[1] = 0.0;
      circle.color[2] = 0.0;

      glGenVertexArrays(1, &circle.vao);
      glGenBuffers(1, &circle.vbo);

      glBindVertexArray(circle.vao);
      glBindBuffer(GL_ARRAY_BUFFER, circle.vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(circle.verts), circle.verts, GL_STREAM_DRAW);

      // Position attribute
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
      glEnableVertexAttribArray(0);

      _initialized = true;
    }

    void _create_vertices() {
      circle.verts[0] = 0;
      circle.verts[1] = 0;
      circle.verts[2] = 0;
      for (int i = 1; i < NVertices; i++) {
        circle.verts[i * 3] = static_cast<float> (0 + (1 * cos(i * 2 * M_PI / NSides)));
        circle.verts[i * 3 + 1] = static_cast<float> (0 + (1 * sin(i * 2 * M_PI / NSides)));
        circle.verts[i * 3 + 2] = 0;
      }
    }
  };

  template<unsigned NSides>
  class RenderableMovingBall : public RenderableBall<NSides>, public MovingBall {
    using RenderableBall<NSides>::RenderableBall;
    using MovingBall::MovingBall;
  };

  template<unsigned NLines>
  class Grid {
  public:
    Grid(distance arena_width, distance arena_height, float z = 0.0) :
      arena_width(arena_width), arena_height(arena_height), z(z), _initialized(false) {}

    void draw(Shader &shader) {
      if (!_initialized) _initialize();

      shader.setVec4("color", color[0], color[1], color[2], 1.0);

      glm::mat4 model_matrix(1);
      model_matrix = glm::scale(model_matrix, glm::vec3(arena_width, arena_height, 0));

      shader.setMat4("model_transform", model_matrix);

      // do the actual drawing
      glBindVertexArray(vao);
      glDrawArrays(GL_LINES, 0, NumVertices);
      glBindVertexArray(0);
    }

    ~Grid() {
      if (_initialized) {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
      }
    }

  private:
    static constexpr unsigned NumVertices = 2 * 3 * 2 * NLines;

    distance arena_width;
    distance arena_height;
    GLfloat z;
    GLfloat color[COLOR_LEN];

    GLuint vao;
    GLuint vbo;
    GLfloat vertices[NumVertices];
    bool _initialized;

    void _initialize() {
      _create_vertices();

      color[0] = 0.2;
      color[1] = 0.2;
      color[2] = 0.2;

      glGenVertexArrays(1, &vao);
      glGenBuffers(1, &vbo);

      glBindVertexArray(vao);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

      // Position attribute
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
      glEnableVertexAttribArray(0);

      _initialized = true;
    }

    void _create_vertices() {
      _create_horiz_verts(&vertices[3 * 2 * NLines]);
      _create_vertical_verts(&vertices[0]);
    }

    void _create_vertical_verts(GLfloat verts[]) {
      for (unsigned i = 0; i < NLines; i++) {
        verts[6 * i] = static_cast<GLfloat>(i) / NLines;
        verts[6 * i + 1] = 0;
        verts[6 * i + 2] = z;

        verts[6 * i + 3] = static_cast<GLfloat>(i) / NLines;
        verts[6 * i + 4] = 1;
        verts[6 * i + 5] = z;
      }
    }

    void _create_horiz_verts(GLfloat verts[]) {
      for (unsigned i = 0; i < NLines; i++) {
        verts[6 * i] = 0;
        verts[6 * i + 1] = static_cast<GLfloat>(i) / NLines;
        verts[6 * i + 2] = 0;

        verts[6 * i + 3] = 1;
        verts[6 * i + 4] = static_cast<GLfloat>(i) / NLines;
        verts[6 * i + 5] = 0;
      }
    }
  };

}