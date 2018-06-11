#version 330 core
#include "includes/shared_head.glsl"
#include "includes/fragment_head.glsl"
#include "includes/fragment_main.glsl"

struct Material {
  bool useNormalMap;
  sampler2D normalMap;
};

uniform Material _Material;
uniform vec2 _UVScale;

uniform samplerCube _Cubemap;

in vec2 v_TexCoord;
in mat3 v_TBN;

vec4 frag() {
  vec2 texCoord = v_TexCoord * _UVScale;

  vec3 normal = v_Normal;
  if (_Material.useNormalMap) {
    normal = normalize(texture(_Material.normalMap, texCoord).rgb * 2.0 - 1.0);

    // Transform normal from tangent space (which is what the normal map is)
    // to world space by left-multiplying the world-space basis vectors of
    // this fragment's tangent space.
    normal = normalize(v_TBN * normal);
  }

  vec3 viewDir = normalize(v_ScenePos.xyz - _CameraPosition);
  vec3 reflectDir = reflect(viewDir, normal);
  return texture(_Cubemap, normalize(reflectDir));
}

