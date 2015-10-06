#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "object.h"

struct sphere_t : public object_t {
  sphere_t(void) : object_t() {}

  virtual ~sphere_t(void) {}

  virtual void setup(glm::vec3 pos) override final;
  virtual void teardown(void) override final;

  void update(float dt);
  void render(GLuint shdr_prog);

private:
  struct physical_state_t{
    glm::vec3 force, accl, crnt_vel, prev_vel;
    float mass;
  } state;
  bool check_collisions();

  static uint32_t buf_usage;
  static mesh_t mesh;
  static object_t::gdef_t gfx;
};



#endif
