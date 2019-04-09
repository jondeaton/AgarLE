#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <core/Ball.hpp>
#include <rendering/shader.hpp>

#define CIRCLE_SIDES 50
#define CIRCLE_VERTS (CIRCLE_SIDES + 2)
#define COLOR_LEN 3

namespace Agario {

  template <unsigned NSides>
  struct Circle {
    float verts[3 * (NSides + 1)];
    float color[COLOR_LEN];
    unsigned int vao; // vertex attribute object
    unsigned int vbo; // vertex buffer object (gpu memory)
  };

  template <unsigned NSides>
  class RenderableBall : virtual public Ball {
  public:
    using Ball::Ball;

    void draw(Shader &shader) {
      shader.setVec3("color", circle.color[0], circle.color[1], circle.color[2]);

      // world location
      auto location = glm::vec3(x, y, 0);
      glm::mat4 position_transform(1);
      position_transform = glm::translate(position_transform, location);

      // scaling
      glm::mat4 scale_transform(1);
      scale_transform = glm::scale(scale_transform, glm::vec3(radius(), radius(), 0));

      glm::mat4 model_matrix = position_transform * scale_transform;

      GLint model_loc = glGetUniformLocation(shader.program, "model_transform");
      glUniformMatrix4fv(model_loc, 1, GL_FALSE, &model_matrix[0][0]);

      // draw them!
      glBindVertexArray(circle.vao);
      glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_VERTS);
      glBindVertexArray(0);
    }

  private:
    Circle<NSides> circle;

    void update_verts() {
      circle.verts[0] = 0;
      circle.verts[1] = 0;
      circle.verts[2] = 0;
      for (int i = 1; i < CIRCLE_VERTS; i++) {
        circle.verts[i * 3] = static_cast<float> (0 + (1 * cos(i *  2 * M_PI / NSides)));
        circle.verts[i * 3 + 1] = static_cast<float> (0 + (1 * sin(i * 2 * M_PI / NSides)));
        circle.verts[i * 3 + 2] = 0;
      }
    }
  };

  template <unsigned NSides>
  class RenderableMovingBall : public RenderableBall<NSides>, public MovingBall {
    using RenderableBall<NSides>::RenderableBall;
    using MovingBall::MovingBall;
  };

  template <unsigned NLines>
  class Grid {
  public:
    Grid(Agario::distance arena_width, Agario::distance arena_height) :
      arena_width(arena_width), arena_height(arena_height),
      vao_vertical(0), vao_horizontal(0) {
      _create_vertices();
    }

    void draw(Shader &shader) {
      shader.setVec3("color", color[0], color[1], color[2]);

      glm::mat4 model_matrix(1);
      model_matrix = glm::scale(model_matrix, glm::vec3(arena_width, arena_height, 0));

      GLint model_loc = glGetUniformLocation(shader.program, "model_transform");
      glUniformMatrix4fv(model_loc, 1, GL_FALSE, &model_matrix[0][0]);


      glGenBuffers(1, &vao_horizontal);
      glBindBuffer(GL_ARRAY_BUFFER, vao_horizontal);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vao_horizontal), horizontal_verts, GL_STATIC_DRAW);

      glGenBuffers(1, &vao_vertical);
      glBindBuffer(GL_ARRAY_BUFFER, vao_vertical);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertical_verts), vertical_verts, GL_STATIC_DRAW);

      int num_verts = 2 * 3 * NLines;
      glDrawArrays(GL_LINES, 0, num_verts);
    }

  private:
    Agario::distance arena_width;
    Agario::distance arena_height;

    GLuint vao_vertical;
    GLuint vao_horizontal;

    float color[COLOR_LEN];

    GLfloat vertical_verts[2 * 3 * NLines];
    GLfloat horizontal_verts[2 * 3 * NLines];

    void _create_vertical_verts() {
      // vertical lines
      for (int i = 0; i < NLines; i++) {
        vertical_verts[6 * i] = (float) i / NLines;
        vertical_verts[6 * i + 1] = 0;
        vertical_verts[6 * i + 2] = 0;

        vertical_verts[6 * i + 3] = (float) i / NLines;
        vertical_verts[6 * i + 4] = 1;
        vertical_verts[6 * i + 5] = 0;
      }
    }

    void _create_horiz_verts() {
      // horizontal lines
      for (int i = 0; i < NLines; i++) {
        horizontal_verts[6 * i] = 0;
        horizontal_verts[6 * i + 1] = (float) i / NLines;
        horizontal_verts[6 * i + 2] = 0;

        horizontal_verts[6 * i + 3] = static_cast<GLfloat>(arena_width);
        horizontal_verts[6 * i + 4] = (float) i / NLines;
        horizontal_verts[6 * i + 5] = 0;
      }
    }

    void _create_vertices() {
      _create_vertical_verts();
      _create_horiz_verts();
    }

  };

}