#include "camera.h"
//#include <list>

camera_t::camera_t()
    : matrix(1.0), dir(0.0f), right(0.0f), target(glm::vec3(0.0f, 0.0f, 0.0f)),
      horizontal_ang((float)M_PI), vertical_ang(0.0f), speed(0.0f),
      rotational_speed(0.08f), showreel(false) {}

camera_t::~camera_t(void) { ; }

void camera_t::setup(glm::vec3 pos, float fov, float aspect, float z_near,
                     float z_far) {
  this->pos = pos;
  this->proj = glm::perspective(fov, aspect, z_near, z_far);
  for (int i(0); i < TOTAL; ++i)
    user[i] = false;
}

void camera_t::teardown(void) {}

void camera_t::apply(float dt) {
  // pivot around a particular position i.e. "target"
  if (showreel) {
    static float radius = 24.72f, height = 10.50f, t = 0;
    t += dt;
    this->pos = glm::vec3(radius * cos(float(M_PI * 2.0f) + t), height,
                          radius * sin(float(M_PI * 2.0f) + t));
    this->matrix = glm::lookAt(pos, target, glm::vec3(0.0f, 1.0f, 0.0f));
  } else // move around freely and unrestricted ...
  {
    calc_velocity(dt);
    if (!gui_enabled)
      orient(dt);
  }
}

void camera_t::orient(float dt) {
  static const glm::vec2 centre(window_width / 2, window_height / 2);
  static double xpos = 0, ypos = 0;
  glfwGetCursorPos(window, &xpos, &ypos);

  if (xpos > window_width || xpos < -window_width)
    xpos = window_width / 2;
  if (ypos > window_height || ypos < -window_height)
    ypos = window_height / 2;

  glm::vec2 crnt_pos(xpos, ypos);
  const glm::vec2 diff = (centre - crnt_pos);

  const float max_diff = 20.0f;

  const glm::vec2 ad = glm::abs(diff);
  const glm::vec2 diff_rate = glm::vec2(glm::pow(ad, glm::vec2(2)));

  if (crnt_pos.x > centre.x)
    crnt_pos.x -= diff_rate.x * dt;
  else if (crnt_pos.x < centre.x)
    crnt_pos.x += diff_rate.x * dt;

  if (crnt_pos.y > centre.y)
    crnt_pos.y -= diff_rate.y * dt;
  else if (crnt_pos.y < centre.y)
    crnt_pos.y += diff_rate.y * dt;

  crnt_pos = glm::clamp(crnt_pos, centre - glm::vec2(max_diff),
                        centre + glm::vec2(max_diff));

  glfwSetCursorPos(window, crnt_pos.x, crnt_pos.y);

  float a = diff.x;
  glm::vec2 b = (centre - crnt_pos);

  horizontal_ang += rotational_speed * dt * diff.x;
  if (horizontal_ang > M_PI * 2.0f)
    horizontal_ang = diff.x;
  vertical_ang += rotational_speed * dt * diff.y;
  if (vertical_ang > M_PI * 2.0f)
    vertical_ang = 0;

  this->dir =
      glm::vec3(cos(vertical_ang) * sin(horizontal_ang), sin(vertical_ang),
                cos(vertical_ang) * cos(horizontal_ang));

  // right vector
  this->right = glm::vec3(sin(horizontal_ang - ((float)(M_PI) / 2.0f)), 0,
                          cos(horizontal_ang - ((float)(M_PI) / 2.0f)));

  // up vector : perpendicular to both direction and right
  glm::vec3 up = glm::cross(this->right, this->dir);

  // form "the" camera matrix
  this->matrix = glm::lookAt(pos, pos + dir, up);
}

void camera_t::calc_velocity(float dt) {
  glm::vec3 forward_velocity(dir * dt * speed),
      strafing_velocity(right * dt * speed);

  static bool prev_move_foward = false, prev_move_right = false;

  if (user[FORWARD] || user[BACK] || user[LEFT] || user[RIGHT]) {
    speed += dt * 75.0f; // TODO: fix scaling issue
    speed = glm::clamp(speed, 0.0f, 64.0f);
  } else {
    // apply momentum...
    speed *= 0.97f;

    if (strafing == false) {
      pos += (forward_velocity * ((prev_move_foward) ? 1.0f : -1.0f));
    }

    if (moving_back_or_forth == false) {
      pos += (strafing_velocity * ((prev_move_right) ? 1.0f : -1.0f));
    }
  }

  if (user[FORWARD]) {
    pos += forward_velocity;
    prev_move_foward = true;
  } else if (user[BACK]) {
    pos -= forward_velocity;
    prev_move_foward = false;
  }

  if (user[RIGHT]) {
    pos += strafing_velocity;
    prev_move_right = true;
  } else if (user[LEFT]) {
    pos -= strafing_velocity;
    prev_move_right = false;
  }
}

void camera_t::process_input(int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    switch (key) {
    case GLFW_KEY_W:
      user[FORWARD] = true;
      break;
    case GLFW_KEY_S:
      user[BACK] = true;
      break;
    case GLFW_KEY_D:
      user[RIGHT] = true;
      break;
    case GLFW_KEY_A:
      user[LEFT] = true;
      break;
    case GLFW_KEY_SPACE:
      showreel = showreel ? false : true;
      break;
    }
  } else if (action == GLFW_RELEASE) {
    switch (key) {
    case GLFW_KEY_W:
      user[FORWARD] = false;
      break;
    case GLFW_KEY_S:
      user[BACK] = false;
      break;
    case GLFW_KEY_D:
      user[RIGHT] = false;
      break;
    case GLFW_KEY_A:
      user[LEFT] = false;
      break;
    }

    switch (key) {
    case GLFW_KEY_W:
    case GLFW_KEY_S:
      moving_back_or_forth = true;
      strafing = false;
      break;
    case GLFW_KEY_D:
    case GLFW_KEY_A:
      strafing = true;
      moving_back_or_forth = false;
      break;
    }
  }
}
