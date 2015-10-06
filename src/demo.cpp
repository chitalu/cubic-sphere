#include "demo.h"
#include <cprintf/cprintf.hpp>

#include "tools.h"
#include "camera.h"
#include "cube.h"
#include "sphere.h"
#include <memory>

static GLint shdr_prog = 0;

const char *vs_src = R"vs(
#version 330

uniform mat4 u_mvp;
uniform mat3 u_norm_mtrx;

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_nrm;

out vs_data {
  vec3 colr;
  vec3 norm;
}
output_;

void main(void) {
  output_.norm = u_norm_mtrx * a_nrm;
  output_.colr = normalize(a_pos).xyz;
  gl_Position = u_mvp * vec4(a_pos, 1.0f);
}
)vs";

const char *fs_src = R"vs(
#version 330

in vs_data{
  vec3 colr;
  vec3 norm;
}
input_;

layout(location = 0) out vec4 frag;

void main(void) { frag = vec4(input_.colr, 1.0f); }
)vs";

static std::vector<std::unique_ptr<object_t>> objects;

bool demo_app_t::init(int argc, char const *argv[]) {
  bool rt = true;
  cprintf(L"$c*`begin$? demo setup\n");

  GLuint vs = create_shader(GL_VERTEX_SHADER, vs_src);
  GLuint fs = create_shader(GL_FRAGMENT_SHADER, fs_src);
  shdr_prog = create_shader_program(2, vs, fs);

  glDeleteShader(vs);
  glDeleteShader(vs);

  objects.resize(10);
  int i = -8;
  for (auto &obj : objects) {
    if (i < 0)
      obj = std::unique_ptr<object_t>(new sphere_t);
    else
      obj = std::unique_ptr<object_t>(new cube_t);

    obj->setup({i, 5.0f, i & 1 ? i : -i});
    i += 2;
  }

  if (rt)
    cprintf(L"demo setup $g*success$?`!\n");
  return rt;
}

bool demo_app_t::teardown(void) {
  bool rt = true;
  cprintf(L"$c*`begin$? demo teardown\n");

  for (auto &obj : objects)
    obj->teardown();

  if (rt)
    cprintf(L"demo teardown $g*success$?`!\n");
  return rt;
}

void demo_app_t::update(float dt) {

  for (auto &obj : objects) {
    void *ptr = (void *)dynamic_cast<sphere_t *>(obj.get());
    if (ptr) {
      ((sphere_t *)ptr)->update(dt);
      continue;
    }

    ptr = (void *)dynamic_cast<cube_t *>(obj.get());
    if (ptr) {
      ((cube_t *)ptr)->update(dt);
      continue;
    }

    assert(0 && "Invalid pointer casting!");
  }
}

void demo_app_t::input(int key, int scancode, int action, int mods) {}

void demo_app_t::render(void) {
  // draw cubes

  for (auto &obj : objects) {
    void *ptr = (void *)dynamic_cast<sphere_t *>(obj.get());
    if (ptr) {
      ((sphere_t *)ptr)->render(shdr_prog);
      continue;
    }

    ptr = (void *)dynamic_cast<cube_t *>(obj.get());
    if (ptr) {
      ((cube_t *)ptr)->render(shdr_prog);
      continue;
    }

    assert(0 && "Invalid pointer casting!");
  }
}
