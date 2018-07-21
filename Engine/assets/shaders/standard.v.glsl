#version 330 core
#include "includes/shared_head.glsl"
#include "includes/vertex_head.glsl"
#include "includes/vertex_main.glsl"

uniform vec2 _UVScale;

out VertexData {
  vec2 v_TexCoord;
} vs_out;

vec4 vert() {
  vs_out.v_TexCoord = in_TexCoord * _UVScale;
  return _Matrix_MVP * vec4(in_Position, 1.0);
}

