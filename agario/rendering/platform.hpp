#pragma once

#define GL_SILENCE_DEPRECATION

#ifdef __APPLE__
# define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED

//#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>
#include <OpenGL/gl3.h>

#else

#ifdef _WIN32
  #include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#endif