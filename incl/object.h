#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "base.h"
#include "tools.h"

template <typename T> struct gfx_obj_t {
  typedef T derived_t;
  static constexpr struct {
    uint32_t pos, norm, txcrd, col;
  } vtx_attr = {0U, 1U, 2U, 3U};
  static uint32_t buf_usage;
  static mesh_t mesh;

  struct def_t {
    GLuint vao;
    union {
      GLuint arr[4];
      struct {
        GLuint vtx, idx, txcrd, nrm;
      };
    } bufs; // handles
  };

  static def_t gfx_def;

  gfx_obj_t(void) {}
  ~gfx_obj_t(void) {}

  void define_(const mesh_create_info_t &mci) {
    struct {
      void operator()(GLenum target, GLsizei sz, void *host_ptr) {
        glBufferData(target, sz, NULL, GL_STATIC_DRAW);
        void *gpu_ptr = glMapBuffer(target, GL_WRITE_ONLY);
        if (!gpu_ptr) {
          fprintf(stderr, "ERROR: failed to map buffer\n");
          exit(1);
        }
        memcpy(gpu_ptr, host_ptr, sz);
        GLboolean result = glUnmapBuffer(target);
        if (!result) {
          fprintf(stderr, "ERROR: failed to unmap buffer\n");
          exit(1);
        }
        gpu_ptr = NULL;
      }
    } fill_buf;

    create_mesh_data(&mci, &mesh);

    glGenVertexArrays(1, &gfx_def.vao);
    glGenBuffers(4, (GLuint *)(&gfx_def.bufs));

    glBindVertexArray(gfx_def.vao);

    // vertices
    assert(!mesh.vtx_data.empty() && "Invalid mesh structure!");
    glBindBuffer(GL_ARRAY_BUFFER, gfx_def.bufs.vtx);
    fill_buf(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.vtx_data.size(),
             (GLvoid *)mesh.vtx_data.data());
    glVertexAttribPointer(vtx_attr.pos, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    // indices
    if (!mesh.idx_data.empty()) {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gfx_def.bufs.idx);
      fill_buf(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * mesh.idx_data.size(),
               (GLvoid *)mesh.idx_data.data());
    }

    // normals
    if (!mesh.norm_data.empty()) {
      glBindBuffer(GL_ARRAY_BUFFER, gfx_def.bufs.nrm);
      fill_buf(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.norm_data.size(),
               (GLvoid *)mesh.norm_data.data());
      glVertexAttribPointer(vtx_attr.norm, 3, GL_FLOAT, GL_TRUE, 0, NULL);
    }

    // texture coordinates
    if (!mesh.txcrd_data.empty()) {
      glBindBuffer(GL_ARRAY_BUFFER, gfx_def.bufs.txcrd);
      fill_buf(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh.txcrd_data.size(),
               (GLvoid *)mesh.txcrd_data.data());
      glVertexAttribPointer(vtx_attr.txcrd, 2, GL_FLOAT, GL_TRUE, 0, NULL);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

  void destroy_(void) { glDeleteBuffers(4, (GLuint*)(&gfx_def.bufs)); }
};

struct object_t {

  object_t(void) : mat(glm::mat4(1.0f)), pos(0.0f) {}
  ~object_t(void) {}

  virtual void setup(glm::vec3 pos) = 0;
  virtual void teardown(void) = 0;

  inline const glm::mat4 &get_matrix(void) const { return mat; }

protected:
  glm::mat4 mat;
  glm::vec3 pos;
};

#endif
