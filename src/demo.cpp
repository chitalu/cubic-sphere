#include "demo.h"
#include <cprintf/cprintf.hpp>

#include "tools.h"

bool demo_app_t::init(int argc, char const *argv[]) {
  bool rt = true;
  cprintf(L"$c*`begin$? demo setup\n");

  mesh_t quad_mesh = {};
  mesh_create_info_t mci = {
          .type = mesh_type::QUAD, 
          .sz_param0 = 2,
          .sz_param1 = 2,
  };

  create_mesh_data(&mci, &quad_mesh);

  destroy_mesh_data(&quad_mesh);

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
