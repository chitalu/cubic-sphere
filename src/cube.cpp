#include "cube.h"
#include "tools.h"
#include "camera.h"

template <> uint32_t gfx_obj_t<cube_t>::buf_usage = 0;
template <> mesh_t gfx_obj_t<cube_t>::mesh = {};
template <> gfx_obj_t<cube_t>::def_t gfx_obj_t<cube_t>::gfx_def = {};

void cube_t::setup(glm::vec3 pos) {
  this->pos = pos;
  if (!buf_usage++) {
    mesh_create_info_t mci = {
        .type = mesh_type::CUBE,
        .sz_param0 = 0.5f, // length
        .sz_param1 = 0.5f, // breadth
        .sz_param2 = 0.5f  // depth
    }; 
    gfx_obj_t<cube_t>::define_(mci);
  }
}

void cube_t::teardown(void) {
  if (--buf_usage == 0)
    gfx_obj_t<cube_t>::destroy_();
}

void cube_t::update(float dt) {
  static float t = 0;
  t += dt;
  pos.y = sin(t) + 1.0f;
  mat = glm::translate(glm::mat4(1.0), pos);
}

void cube_t::render(GLuint shdr_prog) {
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
  glEnableVertexAttribArray(vtx_attr.norm);

  glDrawElements(GL_TRIANGLES, mesh.idx_data.size(), GL_UNSIGNED_INT, NULL);

  glDisableVertexAttribArray(vtx_attr.pos);
  glDisableVertexAttribArray(vtx_attr.norm);
  glBindVertexArray(0);
  glUseProgram(0);
}

