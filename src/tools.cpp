
#include "tools.h"

void make_quad(const mesh_create_info_t* info, mesh_t *m) {
  printf("prepare quad mesh\n");
  assert(m != NULL && "null pointer");

  float l = info->sz_param0 / 2.0f;
  float b = info->sz_param1 / 2.0f;

  // vertices
  m->vtx_data = { { -l, 0, -b }, { l, 0, -b }, { -l, 0, b }, { l, 0, b } };

  // indices
  m->idx_data = { 0U, 2U, 3U, /**/ 0U, 3U, 1U };

  // normals
  m->norm_data = { { 0.0f, 1.0f, 0.0f },
                   { 0.0f, 1.0f, 0.0f },
                   { 0.0f, 1.0f, 0.0f },
                   { 0.0f, 1.0f, 0.0f } };

  // texture coordinates
  m->txcrd_data = {
    { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0, 0.0f }, { 1.0f, 0.0f }
  };
}

void make_grid(const mesh_create_info_t* info, mesh_t *m) {
  printf("preparing grid mesh\n");
  assert(m != NULL && "null pointer");

  size_t size_xdim = info->sz_param0, size_zdim = info->sz_param1;

  /*both size parameters must be perfectly divisible by two*/
  assert(((size_xdim % 2) == 0) && ((size_zdim % 2) == 0));

  const uint32_t vtx_cnt = size_xdim * size_zdim;
  const uint32_t hlf_xdim = size_xdim / 2;
  const uint32_t hlf_zdim = size_zdim / 2;

  m->vtx_data.reserve(vtx_cnt);

  for (auto i = 0u; i < vtx_cnt; i++) {
    m->vtx_data.push_back(glm::vec3(0.0, 0.0, 0.0));
  }

  for (auto x = 0u; x < size_xdim; x++) {
    for (auto z = 0u; z < size_zdim; z++) {
      m->vtx_data[x + (z * size_xdim)] =
          glm::vec3((float)x - hlf_xdim, 0.0f, (float)z - hlf_zdim);
    }
  }

  const uint32_t tri_per_sqr = 2u;
  const uint32_t idx_per_sqr = 4u;
  const uint32_t idx_cnt =
      ((size_xdim - 1) * (size_zdim - 1)) * tri_per_sqr * idx_per_sqr;

  m->idx_data.reserve(idx_cnt);

  for (auto x = 0u; x < size_xdim - 1; x++) {
    for (auto z = 0u; z < size_zdim - 1; z++) {
      m->idx_data.push_back(x + (z * size_xdim));
      m->idx_data.push_back(x + (z * size_xdim) + size_xdim);
      m->idx_data.push_back(x + (z * size_xdim) + size_xdim + 1);

      m->idx_data.push_back(x + (z * size_xdim));
      m->idx_data.push_back(x + (z * size_xdim) + size_xdim + 1);
      m->idx_data.push_back((x + 1) + (z * size_xdim));
    }
  }

  for (auto i = 0u; i < vtx_cnt; i++) {
    m->norm_data.push_back(glm::vec3(0.0, 1.0, 0.0));
  }

  m->txcrd_data = {
    { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f }
  };
}

void make_sphere(const mesh_create_info_t* info, mesh_t *m) {
  printf("preparing sphere mesh\n");
  assert(m != NULL && "null pointer");

  float radius = info->sz_param0 / 2.0f;
  int latitudes = info->sz_param1;
  int longitudes = info->sz_param2;

  m->vtx_data.resize(latitudes * longitudes);

  float latitude_increment = 360.0f / latitudes;
  float longitude_increment = 180.0f / longitudes;

  for (float u = 0; u < 360.0f; u += latitude_increment) {
    for (float t = 0; t < 180.0f; t += longitude_increment) {
      float rad = radius;

      float x = (float)(rad * sin(glm::radians(t)) * sin(glm::radians(u)));
      float y = (float)(rad * cos(glm::radians(t)));
      float z = (float)(rad * sin(glm::radians(t)) * cos(glm::radians(u)));

      m->vtx_data.push_back({ x, y, z });

      float x1 = (float)(rad * sin(glm::radians(t + longitude_increment)) *
                         sin(glm::radians(u + latitude_increment)));
      float y1 = (float)(rad * cos(glm::radians(t + longitude_increment)));
      float z1 = (float)(rad * sin(glm::radians(t + longitude_increment)) *
                         cos(glm::radians(u + latitude_increment)));

      m->vtx_data.push_back({ x1, y1, z1 });
    }
  }
}

void make_cube(const mesh_create_info_t *info, mesh_t *m) {
  printf("preparing cube mesh\n");
  assert(m != NULL && "null pointer");

  float size_x = info->sz_param0, size_y = info->sz_param1,
        size_z = info->sz_param2;

  m->vtx_data = {
    // front quad
    { -size_x, -size_y, size_z }, // 0
    { size_x, -size_y, size_z },  // 1
    { size_x, size_y, size_z },   // 2
    { -size_x, size_y, size_z },  // 3

    // back quad
    { -size_x, -size_y, -size_z }, // 4
    { size_x, -size_y, -size_z },  // 5
    { size_x, size_y, -size_z },   // 6
    { -size_x, size_y, -size_z }   // 7
  };

  m->idx_data = {
    // front
    0U, 1U, 2U, /**/ 2U, 3U, 0U,
    // top
    3U, 2U, 6U, /**/ 6U, 7U, 3U,
    // back
    7U, 6U, 5U, /**/ 5U, 4U, 7U,
    // bottom
    4U, 5U, 1U, /**/ 1U, 0U, 4U,
    // left
    4U, 0U, 3U, /**/ 3U, 7U, 4U,
    // right
    1U, 5U, 6U, /**/ 6U, 2U, 1U
  };

  for (uint32_t vertex = 0u; vertex < m->idx_data.size(); vertex++) {
    if (vertex < 6)
      m->norm_data.push_back({ 0.0f, 0.0f, 1.0f }); // front
    if (vertex >= 6 && vertex < 12)
      m->norm_data.push_back({ 0.0f, 1.0f, 0.0f }); // top
    if (vertex >= 12 && vertex < 18)
      m->norm_data.push_back({ 0.0f, 0.0f, -1.0f }); // back
    if (vertex >= 18 && vertex < 24)
      m->norm_data.push_back({ 0.0f, -1.0f, 0.0f }); // bottom
    if (vertex >= 24 && vertex < 30)
      m->norm_data.push_back({ -1.0f, 0.0f, 0.0f }); // left
    if (vertex >= 32 && vertex < 36)
      m->norm_data.push_back({ 1.0f, 0.0f, 0.0f }); // right
  }

  // not perfect
  m->txcrd_data.push_back(glm::vec2(0.0, 0.0));
  m->txcrd_data.push_back(glm::vec2(1.0, 0.0));
  m->txcrd_data.push_back(glm::vec2(1.0, 1.0));
  m->txcrd_data.push_back(glm::vec2(0.0, 1.0));
}

void make_torus(const mesh_create_info_t* info, mesh_t *m) {
  printf("preparing torus mesh\n");
  assert(m != NULL && "null pointer");
}

void create_mesh_data(const mesh_create_info_t *info, mesh_t *m) {
  assert(m != NULL && "null pointer");

  switch (info->type) {
  case QUAD:
    make_quad(info, m);
    break;
  case GRID:
    make_grid(info, m);
    break;
  case DISC:
    make_grid(info, m);
    break;
  case SPHERE:
    make_sphere(info, m);
    break;
  case CUBE:
    make_cube(info, m);
    break;
  case TORUS:
    make_torus(info, m);
    break;
  default:
    assert(0 && "Invalid mesh type!");
  };

  auto print = [](size_t count, size_t type_size, const char *data) {
    printf("... number of %s: %lu [%.2f Mb]", data, count,
           (float)(count * type_size) / 2048.0f);
  };

  print(m->vtx_data.size(), sizeof(glm::vec3), "vertices");
  print(m->idx_data.size(), sizeof(uint32_t), "indices");
  print(m->txcrd_data.size(), sizeof(glm::vec2), "tex-coords");
  print(m->norm_data.size(), sizeof(glm::vec3), "normals");
}

void destroy_mesh_data(mesh_t *ptr) {
  if (!ptr->vtx_data.empty()) {
    ptr->vtx_data.resize(0);
    ptr->vtx_data.shrink_to_fit();
  }

  if (!ptr->idx_data.empty()) {
    ptr->idx_data.resize(0);
    ptr->idx_data.shrink_to_fit();
  }

  if (!ptr->txcrd_data.empty()) {
    ptr->txcrd_data.resize(0);
    ptr->txcrd_data.shrink_to_fit();
  }

  if (!ptr->norm_data.empty()) {
    ptr->norm_data.resize(0);
    ptr->norm_data.shrink_to_fit();
  }
}

