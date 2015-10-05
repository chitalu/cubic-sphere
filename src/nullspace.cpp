#include "nullspace.h"
#include "base.h"
#include "camera.h"

static const char *vs_src = ""
                            "#version 330 core\n"
                            "uniform mat4 u_mvp;\n"
                            "layout(location = 0) in vec4 a_pos;\n"
                            "void main(void) {\n"
                            "  gl_Position = (u_mvp * a_pos);\n"
                            "}\n";

static const char *fs_src =
    ""
    "#version 330 core\n"
    "uniform vec3 u_color = vec3(1, 1, 0);\n"
    "layout(location = 0) out vec4 fragment;\n"
    "void main(void) { fragment = vec4(u_color, 1.0f); }\n";

GLuint vtx_buf;
GLuint vtx_arr;
static GLuint shdr_prog;

int sz = 8, num_grid_verts = sz * (2 * 4), num_border_verts = 4,
    num_axes_verts = 6,
    num_verts = (num_grid_verts + num_border_verts + num_axes_verts);

struct draw_array_t {
  GLenum mode;
  GLint first;
  GLsizei count;
  float line_width;
  glm::vec3 color;
};

draw_array_t draw_arrays[] = {
  { GL_LINES, 0, (GLsizei)num_grid_verts, 1.0f, glm::vec3(0) },        // grid
  { GL_LINE_LOOP, 0, (GLsizei)num_border_verts, 10.0f, glm::vec3(1) }, // border
  { GL_LINES, 0, 2, 5.0f, glm::vec3(1, 0, 0) },                        // x axis
  { GL_LINES, 0, 2, 5.0f, glm::vec3(0, 1, 0) },                        // y axis
  { GL_LINES, 0, 2, 5.0f, glm::vec3(0, 0, 1) },                        // z axis
};

const uint32_t num_draw_arrays = sizeof(draw_arrays) / sizeof(draw_array_t);

void nullspace_init(void) {
  GLint vs = create_shader(GL_VERTEX_SHADER, vs_src);
  GLint fs = create_shader(GL_FRAGMENT_SHADER, fs_src);
  shdr_prog = create_shader_program(2, vs, fs);

  glDeleteShader(vs);
  glDeleteShader(fs);

  glGenVertexArrays(1, &vtx_arr);
  glGenBuffers(1, &vtx_buf);

  glBindVertexArray(vtx_arr);
  {
    glBindBuffer(GL_ARRAY_BUFFER, vtx_buf);
    {
      glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * num_verts, NULL,
                   GL_STATIC_DRAW);

      glm::vec4 *ptr = (glm::vec4 *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
      if (!ptr) {
        fprintf(stderr, "ERROR: failed to map null-space vertex buffer\n");
        exit(1);
      }

      // grid (GL_LINES)
      size_t idx = 0;
      for (int i = -sz; i < sz; ++i) {
        ptr[idx++] = glm::vec4(-sz, 0, i, 1);
        ptr[idx++] = glm::vec4(sz, 0, i, 1);

        ptr[idx++] = glm::vec4(i, 0, -sz, 1);
        ptr[idx++] = glm::vec4(i, 0, sz, 1);
      }

      // border line (GL_LINE_LOOP)
      ptr[idx++] = glm::vec4(-sz, 0, -sz, 1);
      ptr[idx++] = glm::vec4(-sz, 0, sz, 1);
      ptr[idx++] = glm::vec4(sz, 0, sz, 1);
      ptr[idx++] = glm::vec4(-sz, 0, sz, 1);

      /* axis lines (GL_LINES)*/

      // x-axis
      ptr[idx++] = glm::vec4(-sz, 0, 0, 1);
      ptr[idx++] = glm::vec4(sz, 0, 0, 1);

      // y-axis
      ptr[idx++] = glm::vec4(0, sz, 0, 1);
      ptr[idx++] = glm::vec4(0, 0, 0, 1);

      // z-axis
      ptr[idx++] = glm::vec4(0, 0, -sz, 1);
      ptr[idx++] = glm::vec4(0, 0, sz, 1);

      GLboolean result = glUnmapBuffer(GL_ARRAY_BUFFER);
      if (!result) {
        fprintf(stderr, "ERROR: failed to unmap null-space vertex buffer\n");
        exit(1);
      }
      ptr = NULL;

      glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    }
    glBindVertexArray(0);
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // buffer lookup offsets upon render
  size_t offset = 0;
  for (uint32_t i(0); i < num_draw_arrays; ++i) {
    draw_array_t *draw_array = &draw_arrays[i];
    draw_array->first = offset;
    offset += draw_array->count;
  }
}

void nullspace_teardown(void) {
  glDeleteBuffers(1, &vtx_buf);
  glDeleteVertexArrays(1, &vtx_arr);
  glDeleteProgram(shdr_prog);
}

void nullspace_render(void) {
  GLint last_texture, last_array_buffer, last_vertex_array,
      last_element_array_buffer, last_program;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
  glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
  glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
  GLboolean depth_test_was_enabled = glIsEnabled(GL_DEPTH_TEST);

  if (!depth_test_was_enabled)
    glEnable(GL_DEPTH_TEST);

  glUseProgram(shdr_prog);
  glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0)),
            mvp = cam.get_proj() * cam.get_matrix() * model;

  GLint location = glGetUniformLocation(shdr_prog, "u_mvp");
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mvp));

  glBindVertexArray(vtx_arr);
  glEnableVertexAttribArray(0);

  GLfloat last_line_width;
  glGetFloatv(GL_LINE_WIDTH, &last_line_width);

  for (uint32_t i(0); i < num_draw_arrays; ++i) {
    draw_array_t *draw_array = &draw_arrays[i];

    if (i)
      glDisable(GL_DEPTH_TEST);

    location = glGetUniformLocation(shdr_prog, "u_color");
    glUniform3fv(location, 1, glm::value_ptr(draw_array->color));

    glLineWidth(draw_array->line_width);

    glDrawArrays(draw_array->mode, draw_array->first, draw_array->count);
  }

  glLineWidth(last_line_width);

  glDisableVertexAttribArray(0);
  glBindVertexArray(0);
  glUseProgram(0);

  if (depth_test_was_enabled)
    glEnable(GL_DEPTH_TEST);
  else
    glDisable(GL_DEPTH_TEST);

  // Restore modified GL state
  glUseProgram(last_program);
  glBindTexture(GL_TEXTURE_2D, last_texture);
  glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
  glBindVertexArray(last_vertex_array);
}
