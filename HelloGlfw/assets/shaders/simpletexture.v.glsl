#version 330 core

uniform mat4 MATRIX_MVP;

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec2 in_TexCoord;

out vec2 varying_TexCoord;

void main() {
  gl_Position = MATRIX_MVP * vec4(in_Position, 1.0);
  varying_TexCoord = in_TexCoord;
}

