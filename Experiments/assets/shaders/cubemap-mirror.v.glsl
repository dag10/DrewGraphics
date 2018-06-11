#version 330 core
#include "includes/shared_head.glsl"
#include "includes/vertex_head.glsl"
#include "includes/vertex_main.glsl"

vec4 vert() {
  return _Matrix_MVP * vec4(in_Position, 1.0);
}

