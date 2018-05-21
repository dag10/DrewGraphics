#version 330 core
#include "includes/shared_head.glsl"
#include "includes/vertex_head.glsl"
#include "includes/vertex_main.glsl"

out vec2 v_TexCoord;
out vec3 v_Position;

vec4 vert() {
  v_TexCoord = in_TexCoord;
  vec4 pos = _Matrix_M * vec4(in_Position, 1.0);
  v_Position = pos.xyz / pos.w;
  return _Matrix_MVP * vec4(in_Position, 1.0);
}

