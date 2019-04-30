#pragma once

#include "rendering/platform.hpp"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <exception>
#include <vector>
#include <string>
#include <math.h>
#include <optional>

#include "engine/GameState.hpp"
#include <core/Entities.hpp>
#include <core/Player.hpp>

#include "rendering/Canvas.hpp"
#include "rendering/shader.hpp"


#define NUM_GRID_LINES 11

const char* vertex_shader_src =
#include "shaders/_vertex.glsl"
  ;

const char* fragment_shader_src =
#include "shaders/_fragment.glsl"
  ;

namespace agario {

  class Renderer {
  public:

    typedef Player<true> Player;
    typedef Cell<true> Cell;
    typedef Pellet<true> Pellet;
    typedef Food<true> Food;
    typedef Virus<true> Virus;

    explicit Renderer(std::shared_ptr<Canvas> canvas,
                      agario::distance arena_width,
                      agario::distance arena_height) :
      canvas(std::move(canvas)),
      arena_width(arena_width), arena_height(arena_height),
      shader(), grid(arena_width, arena_height) {

      shader.compile_shaders(vertex_shader_src, fragment_shader_src);
      shader.use();
    }

    explicit Renderer(agario::distance arena_width, agario::distance arena_height) :
      Renderer(nullptr, arena_width, arena_height) {}

    /**
     * converts a screen position to a world position
     * @param player player to calculate position relative to
     * @param xpos screen horizontal position (0 to screen_width - 1)
     * @param ypos screen vertical position (0 to screen_height - 1)
     * @return world location
     */
    agario::Location to_target(Player &player, float xpos, float ypos) {

      // normalized device coordinates (from -1 to 1)
      auto ndc_x = 2 * (xpos / canvas->width()) - 1;
      auto ndc_y = 1 - 2 * (ypos / canvas->height());
      auto loc = glm::vec4(ndc_x, ndc_y, 1.0, 1);

      auto perspective = perspective_projection(player);
      auto view = view_projection(player);

      auto world_loc = glm::inverse(perspective * view) * loc;
      auto w = world_loc[3];
      auto x = world_loc[0] / w;
      auto y = world_loc[1] / w;

      return agario::Location(x, y);
    }

    void make_projections(const Player &player) {
      shader.setMat4("projection_transform", perspective_projection(player));
      shader.setMat4("view_transform", view_projection(player));
    }

    GLfloat camera_z(const Player &player) {
      return 150 + player.mass() / 10.0;
    }

    glm::mat4 perspective_projection(const Player &player) {
      return glm::perspective(glm::radians(45.0f), canvas->aspect_ratio(), 0.1f, 1 + camera_z(player));
    }

    glm::mat4 view_projection(const Player &player) {
      return glm::lookAt(
        glm::vec3(player.x(), player.y(), camera_z(player)), // Camera location in World Space
        glm::vec3(player.x(), player.y(), 0), // camera "looks at" location
        glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
      );
    }

    void render_screen(Player &player, agario::GameState<true> &state) {
      shader.use();

      make_projections(player);

      glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      grid.draw(shader);

      for (auto &pellet : state.pellets)
        pellet.draw(shader);

      for (auto &food : state.foods)
        food.draw(shader);

      for (auto &pair : state.players)
        pair.second->draw(shader);

      for (auto &virus : state.viruses)
        virus.draw(shader);
    }

    ~Renderer() {
      glfwTerminate();
    }

  private:
    std::shared_ptr<Canvas> canvas;

    agario::distance arena_width;
    agario::distance arena_height;

    Shader shader;
    agario::Grid<NUM_GRID_LINES> grid;
  };

}
