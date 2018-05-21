#version 330 core
#include "includes/shared_head.glsl"
#include "includes/fragment_head.glsl"
#include "includes/fragment_main.glsl"

struct Material {
  bool lit;

  bool useDiffuseMap;
  sampler2D diffuseMap;
  vec4 diffuse;

  bool useSpecularMap;
  sampler2D specularMap;
  vec3 specular;

  bool useNormalMap;
  sampler2D normalMap;

  float shininess;
};

uniform Material _Material;
uniform vec2 _UVScale;

in vec2 v_TexCoord;
in mat3 v_TBN;
in vec3 v_Position;

// xyz = world-space position, w = lit (bool)
layout (location = 1) out vec4 g_Position_WS;
// xyz = normal
layout (location = 2) out vec3 g_Normal;
// rgb = specular, a = shininess
layout (location = 3) out vec4 g_Specular;
// xyz = view-space position
layout (location = 4) out vec3 g_Position_VS;

vec4 frag() {
  g_Position_WS = vec4(v_Position, _Material.lit);
  g_Position_VS = (_Matrix_V * vec4(v_Position, 1)).xyz;

  vec2 texCoord = v_TexCoord * _UVScale;

  g_Specular = vec4(_Material.useSpecularMap
      ? texture(_Material.specularMap, texCoord).rgb
      : vec3(_Material.specular), _Material.shininess);

  vec3 normal = v_Normal;
  if (_Material.useNormalMap) {
    normal = normalize(texture(_Material.normalMap, texCoord).rgb * 2.0 - 1.0);

    // Transform normal from tangent space (which is what the normal map is)
    // to world space by left-multiplying the world-space basis vectors of
    // this fragment's tangent space.
    normal = normalize(v_TBN * normal);
  }

  g_Normal = normal;

  return _Material.useDiffuseMap
    ? texture(_Material.diffuseMap, texCoord)
    : vec4(_Material.diffuse);
}

