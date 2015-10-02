#include "demo.h"
#include <cprintf/cprintf.hpp>

bool demo_app_t::init(int argc, char const *argv[]) {
  bool rt = true;
  cprintf(L"$c*`begin$? demo setup\n");

  if (rt)
    cprintf(L"demo setup $g*success$?`!\n");
  return rt;
}

bool demo_app_t::teardown(void) {
  bool rt = true;
  cprintf(L"$c*`begin$? demo teardown\n");

  if (rt)
    cprintf(L"demo teardown $g*success$?`!\n");
  return rt;
}

void demo_app_t::update(float dt) {}

void demo_app_t::input(int key, int scancode, int action, int mods) {}

void demo_app_t::render(void) {}
