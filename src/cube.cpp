#include "cube.h"
#include "tools.h"
#include "camera.h"

uint32_t cube_t::buf_usage = 0;
mesh_t cube_t::mesh = {};
object_t::gdef_t cube_t::gfx = {};

void cube_t::setup(glm::vec3 pos) {
  this->pos = pos;
  if (!buf_usage++) {
    mesh_create_info_t mci = {
        .type = mesh_type::CUBE,
        .sz_param0 = 0.5f, // length
        .sz_param1 = 0.5f, // breadth
        .sz_param2 = 0.5f  // depth
    };
    create_mesh_data(&mci, &mesh);
    this->init_static_bufs_(mesh, gfx);
  }
}

void cube_t::teardown(void) {
  if (--buf_usage == 0)
    this->destroy_static_bufs_(gfx);
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

  glBindVertexArray(gfx.vao);
  glEnableVertexAttribArray(gdef_t::vattr::pos);
  glEnableVertexAttribArray(gdef_t::vattr::norm);

  glDrawElements(GL_TRIANGLES, mesh.idx_data.size(), GL_UNSIGNED_INT, NULL);

  glDisableVertexAttribArray(gdef_t::vattr::pos);
  glDisableVertexAttribArray(gdef_t::vattr::norm);
  glBindVertexArray(0);
  glUseProgram(0);
}


