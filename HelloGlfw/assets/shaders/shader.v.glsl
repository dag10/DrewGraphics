#version 330 core

in vec3 in_Position;
in vec3 in_Color;

out vec3 varying_Color;

void main() {
  gl_Position = vec4(in_Position.x, in_Position.y, in_Position.z, 1.0);
  varying_Color = in_Color;
}

