struct Material {
  bool lit;

  bool useDiffuseMap;
  sampler2D diffuseMap;
  vec3 diffuse;

  bool useSpecularMap;
  sampler2D specularMap;
  vec3 specular;

  bool useNormalMap;
  sampler2D normalMap;

  float shininess;
};

uniform Material _Material;
uniform vec2 _UVScale;

// TODO: TEMPORARY
uniform bool _DepthOnly;

uniform sampler2D _ShadowMap;

in vec2 v_TexCoord;
in mat3 v_TBN;
in vec4 v_FragPosLightSpace;

#define near_plane 0.1
#define far_plane 100.0

float LinearizeDepth(float depth) {
  float z = depth * 2.0 - 1.0; // Back to NDC
  return (2.0 * near_plane * far_plane) /
    (far_plane + near_plane - z * (far_plane - near_plane));
}

float calculateShadow(vec4 fragPosLightSpace) {
  //vec2 shadowUV = v_FragPosLightSpace.xy / v_FragPosLightSpace.w;
  //shadowUV += 1;
  //shadowUV *= 0.5;

  //return texture(_ShadowMap, shadowUV).r;

  // perform perspective divide
  vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
  // transform to [0,1] range
  projCoords = projCoords * 0.5 + 0.5;
  // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
  float closestDepth = texture(_ShadowMap, projCoords.xy).r;
  //closestDepth = LinearizeDepth(closestDepth);
  // get depth of current fragment from light's perspective
  float currentDepth = projCoords.z;
  // check whether current frag pos is in shadow
  float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

  //shadow = currentDepth - closestDepth;

  return shadow;
}

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
  //float shadow = calculateShadow(v_FragPosLightSpace);
  // perform perspective divide
  vec3 projCoords = v_FragPosLightSpace.xyz / v_FragPosLightSpace.w;
  // transform to [0,1] range
  projCoords = projCoords * 0.5 + 0.5;
  // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
  float closestDepth = texture(_ShadowMap, projCoords.xy).r;
  //closestDepth = LinearizeDepth(closestDepth);
  // get depth of current fragment from light's perspective
  float currentDepth = projCoords.z;
  // check whether current frag pos is in shadow
  float bias = max(0.01 * (1.0 - dot(norm, lightDir)), 0.005);
  //bias = 0.005;
  bias = 0.0001;
  float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

  //vec3 lightSpace = v_FragPosLightSpace.xyz / v_FragPosLightSpace.w;
  //lightSpace += 1;
  //lightSpace *= 0.5;

  if (!_DepthOnly) {
    ////return vec3(lightSpace.xy, 0);
    //return texture(_ShadowMap, lightSpace.xy).xyz;
    ////return mix(
        ////diffuseColor,
        ////texture(_ShadowMap, lightSpace.xy).xyz,
        ////0.8);
    ////return mix(
        ////diffuseColor,
        ////lightSpace.zzz,
        ////0.8);

    //return vec3(shadow);
  }

	return ((1.0 - shadow) * (specular + diffuse)) + ambient;
}

vec4 frag() {
  vec2 texCoord = v_TexCoord * _UVScale;

  vec3 diffuseColor = _Material.useDiffuseMap
                    ? texture(_Material.diffuseMap, texCoord).rgb
                    : vec3(_Material.diffuse);

  if (!_Material.lit) {
    return vec4(diffuseColor, 1.0);
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
        _Lights[i], normal, diffuseColor, specularColor);
  }

  return vec4(cumulative, 1.0);
}

