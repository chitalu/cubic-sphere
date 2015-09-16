#version 150 core

uniform vec3 color = vec3(1);
out vec4 fragment;

void main(void) { fragment = vec4(color, 1.0f); }
