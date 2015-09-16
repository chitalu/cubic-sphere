#ifndef __camera_H__
#define __camera_H__

#include "base.h"

struct camera_t {
private:
  glm::mat4 proj, matrix;
  glm::vec3 pos, dir, right,
      // the look-at, as well as pivot-around position
      target;
  float horizontal_ang, // horizontal angle : toward -Z
      // vertical angle : 0, look at the horizon
      vertical_ang, speed, rotational_speed;
  enum INPUT {
    FORWARD = 0,
    BACK,
    LEFT,
    RIGHT,
    SHOWREEL,
    ROTATE,
    TOTAL
  };
  bool user[TOTAL];
  bool showreel, moving_back_or_forth, // forward or backward...
      strafing, rotating;              // left or right ...

  void calc_velocity(float dt);
  void orient(float dt);

public:
  camera_t();
  ~camera_t(void);

  void setup(glm::vec3 pos, float fov, float aspect, float near_plane,
             float far_plane);
  void teardown(void);

  void apply(float dt);

  // return projection matrix
  inline const glm::mat4 &get_proj(void) const { return proj; }

  inline virtual const glm::mat4 &get_matrix(void) const { return matrix; }

  // where I'm I looking?
  inline void set_target(glm::vec3 const &t) { target = t; }

  const glm::vec3 &get_pos(void) const { return this->pos; }

  void rotate(float angle, glm::vec3 const &axes) {
    matrix = glm::translate(glm::mat4(1.0), this->pos);
    matrix *= glm::rotate(matrix, glm::radians(angle), axes);
  }

  void process_input(int key, int scancode, int action, int mods);
};

// initial definition in main.cpp i.e. camera_t cam;
extern camera_t cam;

#endif
