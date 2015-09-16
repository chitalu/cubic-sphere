#version 150 core

uniform mat4 u_mvp;

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_col;

out vec3 color;

void main(void) {
  color = a_col;
  gl_Position = (u_mvp * vec4(a_pos, 1.0f));
}
