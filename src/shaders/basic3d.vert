#version 150 core

uniform mat4 u_mvp;

layout(location = 0) in vec3 a_pos;

void main(void) {
  gl_Position = (u_mvp * vec4(a_pos, 1.0f));
}
