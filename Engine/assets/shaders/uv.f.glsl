#version 330 core
#include "includes/shared_head.glsl"
#include "includes/fragment_head.glsl"
#include "includes/fragment_main.glsl"

in vec2 v_TexCoord;

vec4 frag() {
  return vec4(v_TexCoord.x, v_TexCoord.y, 0, 1);
}

