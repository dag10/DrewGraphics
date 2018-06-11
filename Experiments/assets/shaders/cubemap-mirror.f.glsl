#version 330 core
#include "includes/shared_head.glsl"
#include "includes/fragment_head.glsl"
#include "includes/fragment_main.glsl"

uniform samplerCube skybox;

vec4 frag() {
  vec3 normal = normalize(v_Normal);
  vec3 viewDir = normalize(v_ScenePos.xyz - _CameraPosition );
  vec3 reflectDir = reflect(viewDir, normal);
  return texture(skybox, normalize(reflectDir));
}

