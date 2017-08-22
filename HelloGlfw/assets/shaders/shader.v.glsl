#version 330 core

uniform mat4 MATRIX_MVP;

in vec3 in_Position;
in vec3 in_Color;
in vec2 in_TexCoord;

out vec3 varying_Color;
out vec2 varying_TexCoord;

void main() {
  gl_Position = MATRIX_MVP * vec4(in_Position, 1.0);
  varying_Color = in_Color;
  varying_TexCoord = in_TexCoord;
}

