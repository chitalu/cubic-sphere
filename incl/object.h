#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "base.h"
#include "tools.h"

struct object_t {
  // "graphics" definition of an object
  struct gdef_t {
    enum vattr {
      pos = 0,
      norm = 1,
      txcrd = 2,
      col = 3
    };
    GLuint vao;
    union {
      GLuint arr[4];
      struct {
        GLuint vtx, idx, txcrd, nrm;
      };
    } bufs; // handles
  };

  object_t(void) : mat(glm::mat4(1.0f)), pos(0.0f) {}
   ~object_t(void) {}

  virtual void setup(glm::vec3 pos) =0;
  virtual void teardown(void)=0;

  inline const glm::mat4 &get_matrix(void) const { return mat; }

  void init_static_bufs_(mesh_t &mesh, gdef_t &graphics) {

    auto fill_buf = [&](GLenum target, GLsizei sz, void *host_ptr) {
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
    };

    glGenVertexArrays(1, &graphics.vao);
    glGenBuffers(4, graphics.bufs.arr);

    glBindVertexArray(graphics.vao);

    // vertices
    glBindBuffer(GL_ARRAY_BUFFER, graphics.bufs.vtx);
    fill_buf(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.vtx_data.size(),
             (GLvoid *)mesh.vtx_data.data());
    glVertexAttribPointer(gdef_t::vattr::pos, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    // indices
    if (!mesh.idx_data.empty()) {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, graphics.bufs.idx);
      fill_buf(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * mesh.idx_data.size(),
               (GLvoid *)mesh.idx_data.data());
    }

    // normals
    if (!mesh.norm_data.empty()) {
      glBindBuffer(GL_ARRAY_BUFFER, graphics.bufs.nrm);
      fill_buf(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.norm_data.size(),
               (GLvoid *)mesh.norm_data.data());
      glVertexAttribPointer(gdef_t::vattr::norm, 3, GL_FLOAT, GL_TRUE, 0, NULL);
    }

    // texture coordinates
    if (!mesh.txcrd_data.empty()) {
      glBindBuffer(GL_ARRAY_BUFFER, graphics.bufs.txcrd);
      fill_buf(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh.txcrd_data.size(),
               (GLvoid *)mesh.txcrd_data.data());
      glVertexAttribPointer(gdef_t::vattr::txcrd, 2, GL_FLOAT, GL_TRUE, 0,
                            NULL);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

  void destroy_static_bufs_(gdef_t &graphics) {
    glDeleteBuffers(4, graphics.bufs.arr);
  }

protected:
  glm::mat4 mat;
  glm::vec3 pos;
};

#endif
