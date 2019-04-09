#pragma once

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
  class RenderableBall : public Ball {
  public:
    using Ball::Ball;

    void draw(Shader &shader) {
      shader.setVec3("color", circle.color[0], circle.color[1], circle.color[2]);

      // position/location
      auto location = glm::vec3(x, y, 0);
      glm::mat4 position_transform(1);
      position_transform = glm::translate(position_transform, location);
      GLint location_transform = glGetUniformLocation(shader.program, "position_translation");
      glUniformMatrix4fv(location_transform, 1, GL_FALSE, &position_transform[0][0]);

      // scaling
      glm::mat4 scale_transform(1);
      scale_transform = glm::scale(scale_transform, glm::vec3(radius(), radius(), 0));
      GLint scale_location = glGetUniformLocation(shader.program, "scale_transform");
      glUniformMatrix4fv(scale_location, 1, GL_FALSE, &scale_transform[0][0]);

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
        circle.verts[i * 3] = static_cast<float> (0 + (radius() * cos(i *  2 * M_PI / NSides)));
        circle.verts[i * 3 + 1] = static_cast<float> (0 + (radius() * sin(i * 2 * M_PI / NSides)));
        circle.verts[i * 3 + 2] = 0;
      }
    }
  };

  template <unsigned NSides>
  class RenderableMovingBall : public RenderableBall<NSides>, public MovingBall { };



  template <unsigned GridSpacing>
  class Grid {
  public:
    Grid(Agario::distance arena_width, Agario::distance arena_height) :
      arena_width(arena_width), arena_height(arena_height),
      vbo_vertical(0), vbo_horizontal(0) {
      _create_vertices();
    }

    void draw() {

    }

  private:
    Agario::distance arena_width;
    Agario::distance arena_height;

    GLuint vbo_vertical;
    GLuint vbo_horizontal;

    void _create_vertical_verts() {
      int num_vert_lines = static_cast<int>(arena_width / GridSpacing);

      // allocate vector of vertices
      auto verts = new GLfloat[2 * 3 * num_vert_lines];

      // vertical lines
      for (int i = 0; i < num_vert_lines; i++) {
        verts[6 * i] = i * GridSpacing;
        verts[6 * i + 1] = 0;
        verts[6 * i + 2] = 0;

        verts[6 * i + 3] = i * GridSpacing;
        verts[6 * i + 4] = static_cast<GLfloat>(arena_height);
        verts[6 * i + 5] = 0;
      }

      glGenBuffers(1, &vbo_vertical);
      glBindBuffer(GL_ARRAY_BUFFER, vbo_vertical);
      glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

      delete[] verts;
    }

    void _create_horiz_verts() {
      int num_horz_lines = static_cast<int>(arena_height / GridSpacing);
      auto verts = new GLfloat[2 * 3 * num_horz_lines];

      // horizontal lines
      for (int i = 0; i < num_horz_lines; i++) {
        verts[6 * i] = 0;
        verts[6 * i + 1] = i * GridSpacing;
        verts[6 * i + 2] = 0;

        verts[6 * i + 3] = static_cast<GLfloat>(arena_width);
        verts[6 * i + 4] = i * GridSpacing;
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

}