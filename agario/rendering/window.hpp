#pragma once

#define GL_SILENCE_DEPRECATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <OpenGL/OpenGL.h>

#include <exception>
#include <iostream>

#define WINDOW_NAME "AgarIO"

#define DEFAULT_SCREEN_WIDTH 640
#define DEFAULT_SCREEN_HEIGHT 480

namespace agario {

  class WindowException : public std::runtime_error {
    using runtime_error::runtime_error;
  };

  void glfw_error_callback(int error, const char *description) {
    (void) error;
    throw WindowException(description);
  }

  class Window {
  public:

    Window(int screen_width, int screen_height) :
      screen_width(screen_width), screen_height(screen_height) {

      glfwSetErrorCallback(glfw_error_callback);

      if (!glfwInit())
        throw WindowException("GLFW initialization failed.");

      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

      // Create a windowed mode window and its OpenGL context
      window = glfwCreateWindow(screen_width, screen_height, WINDOW_NAME, nullptr, nullptr);

      if (window == nullptr) {
        glfwTerminate();
        throw WindowException("Window creation failed");
      }

      glfwMakeContextCurrent(window);
      glewExperimental = GL_TRUE;

      GLenum err = glewInit();
      if (err != GLEW_OK)
        throw WindowException("GLEW initialization failed");

      glfwSetWindowUserPointer(window, this);
      glfwSetWindowSizeCallback(window, window_resize_callback);
    }

    Window() : Window(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT) {}

    int width() const {
      return screen_width;
    }

    int height() const {
      return screen_height;
    }

    GLFWwindow *pointer() const {
      return window;
    }

    bool should_close() const {
      return glfwWindowShouldClose(window);
    }

    void swap_buffers() const {
      glfwSwapBuffers(window);
    };

    float aspect_ratio() const {
      return (float) screen_width / (float) screen_height;
    }

    ~Window() {
      glfwDestroyWindow(window);
    }

  private:

    static void window_resize_callback(GLFWwindow *window, int width, int height) {
      auto win = static_cast<Window *>(glfwGetWindowUserPointer(window));
      win->screen_width = width;
      win->screen_height = height;
    }

    GLFWwindow *window;
    int screen_width;
    int screen_height;
  };


}
