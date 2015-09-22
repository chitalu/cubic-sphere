#ifndef __BASE_H__
#define __BASE_H__

// Starting with version 3.0, the GLU header glu.h is no 
// longer included by default. If you wish to include it, 
// define GLFW_INCLUDE_GLU before the inclusion of the GLFW header.
// #define GLFW_INCLUDE_GLU

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp> 
#include <glm/vec4.hpp> 
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cmath>
#include <string>

#include "time-sampler.h"

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#define ENABLE_NULLSPACE 1
#define APP_NAME "OpenGL-Template [BUILT: " __TIME__ "]"

#define glchk_ assert(glGetError() == GL_NO_ERROR);

// window handle as returned by glfw
extern GLFWwindow* window;
extern int window_height;
extern int window_width;

extern double cursor_posx;
extern double cursor_posy;

extern bool gui_enabled;

extern GLuint create_shader(GLenum type, const char *src);
extern GLuint create_shader_program(int shader_count, ...);

struct demo_app_t;

extern demo_app_t* demo;

#endif
