#version 330 core
#include "includes/shared_head.glsl"
#include "includes/fragment_head.glsl"
#include "includes/fragment_main.glsl"

in vec3 v_Direction;

uniform samplerCube skybox;

vec4 frag() {
  return texture(skybox, normalize(v_Direction));
}

