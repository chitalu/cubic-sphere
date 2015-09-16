#include "base.h"

struct demo_app_t {
  bool init(void);
  bool teardown(void);
  void update(float);
  void input(int, int, int, int);
  void render(void);
};
