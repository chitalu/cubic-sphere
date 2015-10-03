
#ifndef __TOOLS_H__
#define __TOOLS_H__

#include "base.h"

struct mesh_t {
  std::vector<glm::vec3> vtx_data;
  std::vector<uint32_t> idx_data;
  std::vector<glm::vec2> txcrd_data;
  std::vector<glm::vec3> norm_data;
};

enum mesh_type {
  QUAD = 0,
  GRID,
  DISC,
  SPHERE,
  CUBE,
  TORUS
};

struct mesh_create_info_t {
  mesh_type type;
  size_t sz_param0, sz_param1, sz_param2;
};

extern void create_mesh_data(mesh_create_info_t *info, mesh_t *out);
extern void destroy_mesh_data(mesh_t *ptr);

#endif
