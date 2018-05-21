#version 330 core
#include "includes/shared_head.glsl"
#include "includes/vertex_head.glsl"
#include "includes/vertex_main.glsl"

out vec2 v_TexCoord;

vec4 vert() {
  v_TexCoord = in_TexCoord;
  return vec4(in_Position.xy, -1, 1);
}

