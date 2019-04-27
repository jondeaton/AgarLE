#pragma once

#define GL_SILENCE_DEPRECATION

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

#include <exception>
#include <iostream>

template<unsigned Width, unsigned Height>
class FrameBufferObject {
public:

  FrameBufferObject() : fbo(0), render_buf(0) {
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &render_buf);
    glBindRenderbuffer(render_buf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, Width, Height);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, render_buf);
  }

  float aspect_ratio() const { return (float) Width / Height; }
  void bind() const { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo); }
  void unbind() const { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); }

  void copy(void *data) {
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0, 0, Width, Height, GL_BGRA,GL_UNSIGNED_BYTE, data);
  }

  ~FrameBufferObject() {
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &render_buf);
  }

private:
  GLuint fbo;
  GLuint render_buf;
};