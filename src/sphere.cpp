#include "sphere.h"
#include "tools.h"
#include "camera.h"

template<>
uint32_t gfx_obj_t<sphere_t>::buf_usage = 0;

template<>
mesh_t  gfx_obj_t<sphere_t>::mesh = {};

template<>
gfx_obj_t<sphere_t>::def_t gfx_obj_t<sphere_t>::gfx_def = {};

void sphere_t::setup(glm::vec3 pos) {
  if (!buf_usage++) {
    const mesh_create_info_t mci = {
        .type = mesh_type::SPHERE,
        .sz_param0 = 2.0f,  // diameter
        .sz_param1 = 32.0f, // lattitude
        .sz_param2 = 32.0f  // longitude
    };
    gfx_obj_t<sphere_t>::define_(mci);
  }

  this->pos = pos;
  state.mass = 0.01f;
  state.force = {0.0f, 0.0f, 0.0f};
  state.accl = {0.0f, 0.0f, 0.0f};
  state.crnt_vel = {0.0f, 0.0f, 0.0f};
  state.prev_vel = {0.0f, 0.0f, 0.0f};
}

void sphere_t::teardown(void) {
  if (--buf_usage == 0)
    gfx_obj_t<sphere_t>::destroy_();
}

bool sphere_t::check_collisions(void) {
  float r = 1.0f; // radius

  // when a sphere is in contact with a plane L (the positive side)
  // The distance from the centre of the sphere P to the plane in the
  // length of the radius
  // L.P = r (writing L as a 4D vector L = <N, D>)
  // The relationship L.P can be written as:
  // N.P + D = r <=> N.P + (D-r) = 0
  // This is the same as saying point P lies on the plane L' given by:
  // L' = <N, D-r>
  // The plane L' is parallel to L
  float D = 0.0f;
  float plane_diff = (D - r);
  glm::vec4 L(0.0f, 1.0f, 0.0f, D), L0 = glm::vec4(glm::vec3(L), plane_diff);

  // if L.P >= r then there is no colision
  bool colliding = (glm::dot(glm::vec3(L), pos) + plane_diff) < r;

  return colliding;
}

void sphere_t::update(float dt) {
  dt = glm::clamp(dt, 0.0f, 0.01f);

  const glm::vec3 fgrav = {0.0f, -9.8f, 0.0f};
  const glm::vec3 fnorm = -fgrav;

  glm::vec3 momentum = state.mass * state.crnt_vel;

  // sum forces
  state.force = fgrav;
  bool colliding = check_collisions();
  if (colliding)
    state.force += fnorm;

  state.accl = (state.force / state.mass);
  state.crnt_vel = (state.prev_vel + state.accl) * dt;
  pos += state.crnt_vel * dt;

  state.prev_vel = state.crnt_vel;
  mat = glm::translate(glm::mat4(1.0f), pos);
}

void sphere_t::render(GLuint shdr_prog) {
  assert(glIsProgram(shdr_prog) && "Invalid program handle!");
  glUseProgram(shdr_prog);

  glm::mat4 mvp = cam.get_proj() * cam.get_matrix() * get_matrix();
  GLint location = glGetUniformLocation(shdr_prog, "u_mvp");
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mvp));

  glm::mat3 normal = glm::transpose(glm::inverse(glm::mat3(get_matrix())));
  location = glGetUniformLocation(shdr_prog, "u_norm_mtrx");
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(normal));

  glBindVertexArray(gfx_def.vao);
  glEnableVertexAttribArray(vtx_attr.pos);
  // glEnableVertexAttribArray(gdef_t::vattr::norm);

  glDrawArrays(GL_LINE_LOOP, 0, mesh.vtx_data.size());

  glDisableVertexAttribArray(vtx_attr.pos);
  // glDisableVertexAttribArray(gdef_t::vattr::norm);
  glBindVertexArray(0);
  glUseProgram(0);
}
