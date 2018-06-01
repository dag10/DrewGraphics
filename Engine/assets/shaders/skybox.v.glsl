#version 330 core
#include "includes/shared_head.glsl"
#include "includes/vertex_head.glsl"
#include "includes/vertex_main.glsl"

out vec3 v_Direction;

vec4 vert() {
  v_Direction = normalize(vec3(
      inverse(_Matrix_MVP) * vec4(in_TexCoord * 2.0 - 1.0, -1.0, 1.0)));
  return vec4(in_Position, 1.0);
}

