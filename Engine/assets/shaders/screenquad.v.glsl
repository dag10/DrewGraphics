#version 330 core
#include "includes/shared_head.glsl"
#include "includes/vertex_head.glsl"
#include "includes/vertex_main.glsl"

uniform vec2 _Scale;
uniform vec2 _Offset;

out vec2 v_TexCoord;

vec4 vert() {
  v_TexCoord = in_TexCoord;
  return vec4(in_Position.xy * _Scale + _Offset, -1, 1);
}

