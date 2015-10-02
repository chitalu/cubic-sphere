#include "base.h"

struct demo_app_t {
  bool init(int argc, char const *argv[]);
  bool teardown(void);
  void update(float);
  void input(int, int, int, int);
  void render(void);
};
