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

uniform sampler2D _ShadowMap;

in vec2 v_TexCoord;
in mat3 v_TBN;
in vec4 v_FragPosLightSpace;

vec3 calculateLight(
    Light light, vec3 normal, vec3 diffuseColor, vec3 specularColor) {
  if (light.type == LIGHT_TYPE_NULL) {
    return vec3(0);
  }

  // Ambient
  vec3 ambient = light.ambient * diffuseColor;

  // Diffuse
  vec3 norm = normalize(normal);
  vec3 lightDir;
  if (light.type == LIGHT_TYPE_POINT || light.type == LIGHT_TYPE_SPOT) {
    lightDir = normalize(light.position - v_ScenePos.xyz);
  } else if (light.type == LIGHT_TYPE_DIRECTIONAL) {
    lightDir = -light.direction;
  }
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = light.diffuse * diff * diffuseColor;

  // Specular
  vec3 viewDir = normalize(_CameraPosition - v_ScenePos.xyz);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), _Material.shininess);
  vec3 specular = light.specular * spec * specularColor;

  // Attenuation
  if (light.type == LIGHT_TYPE_POINT || light.type == LIGHT_TYPE_SPOT) {
    float distance = length(light.position - v_ScenePos.xyz);
    float attenuation =
      1.0 / (light.constantCoeff + light.linearCoeff * distance +
          light.quadraticCoeff * (distance * distance));
    diffuse *= attenuation;
    ambient *= attenuation;
    specular *= attenuation;
  }

  // Spot light cutoff
  if (light.type == LIGHT_TYPE_SPOT) {
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.outerCutoff - light.innerCutoff;
    float intensity = clamp((theta - cos(light.innerCutoff)) / epsilon, 0.0, 1.0);
    diffuse *= intensity;
    specular *= intensity;
  }

  // Calculate shadow
  float shadow = 0;
  if (v_FragPosLightSpace.w != 0) {
    vec3 projCoords = v_FragPosLightSpace.xyz / v_FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(_ShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = 0.0001;
    shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
  }

	return ((1.0 - shadow) * (specular + diffuse)) + ambient;
}

vec4 frag() {
  vec2 texCoord = v_TexCoord * _UVScale;

  vec4 diffuseColor = _Material.useDiffuseMap
                    ? texture(_Material.diffuseMap, texCoord)
                    : vec4(_Material.diffuse);

  if (!_Material.lit) {
    return diffuseColor;
  }

  vec3 specularColor = _Material.useSpecularMap
                     ? texture(_Material.specularMap, texCoord).rgb
                     : vec3(_Material.specular);

  vec3 normal = v_Normal;
  if (_Material.useNormalMap) {
    normal = normalize(texture(_Material.normalMap, texCoord).rgb * 2.0 - 1.0);

    // Transform normal from tangent space (which is what the normal map is)
    // to world space by left-multiplying the world-space basis vectors of
    // this fragment's tangent space.
    normal = normalize(v_TBN * normal);
  }

  vec3 cumulative = vec3(0);
  for (int i = 0; i < MAX_LIGHTS; i++) {
    cumulative += calculateLight(
        _Lights[i], normal, diffuseColor.rgb, specularColor);
  }

  return vec4(cumulative, diffuseColor.a);
}

