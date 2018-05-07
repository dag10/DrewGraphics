uniform sampler2D _AlbedoTexture;
uniform sampler2D _PositionTexture;
uniform sampler2D _NormalTexture;
uniform sampler2D _SpecularTexture;
uniform sampler2D _DepthTexture;
uniform sampler2D _ShadowMap;

uniform mat4 _Matrix_V;

in vec2 v_TexCoord;

vec3 calculateLight(
    Light light, vec3 position, vec3 normal, vec3 diffuseColor, vec3 specularColor,
    float shininess) {
  if (light.type == LIGHT_TYPE_NULL) {
    return vec3(0);
  }

  // Ambient
  vec3 ambient = light.ambient * diffuseColor;

  // Diffuse
  vec3 norm = normalize(normal);
  vec3 lightDir;
  if (light.type == LIGHT_TYPE_POINT || light.type == LIGHT_TYPE_SPOT) {
    lightDir = normalize(light.position - position.xyz);
  } else if (light.type == LIGHT_TYPE_DIRECTIONAL) {
    lightDir = -light.direction;
  }
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = light.diffuse * diff * diffuseColor;

  // Specular
  vec3 viewDir = normalize(_CameraPosition - position.xyz);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
  vec3 specular = light.specular * spec * specularColor;

  // Attenuation
  if (light.type == LIGHT_TYPE_POINT || light.type == LIGHT_TYPE_SPOT) {
    float distance = length(light.position - position.xyz);
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
  if (light.hasShadow == 1) {
    vec4 fragPosLightSpace = light.lightTransform * vec4(position, 1);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(_ShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = 0.0001;
    shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
  }

	return ((1.0 - shadow) * (specular + diffuse)) + ambient;
}

vec4 frag() {
  vec3 normal = texture(_NormalTexture, v_TexCoord).xyz;

  // If no geometry was rendered for this fragment, discard.
  if (length(normal) == 0) {
    discard;
  }

  vec4 albedoTexel = texture(_AlbedoTexture, v_TexCoord);
  vec3 albedo = albedoTexel.rgb;
  float alpha = albedoTexel.a;

  vec4 positionTexel = texture(_PositionTexture, v_TexCoord);
  vec3 position = positionTexel.xyz;
  bool lit = positionTexel.w == 1;

  if (!lit) {
    return vec4(albedo, alpha);
  }

  vec4 specularTexel = texture(_SpecularTexture, v_TexCoord);
  vec3 specular = specularTexel.rgb;
  float shininess = specularTexel.a;

  float depth = texture(_DepthTexture, v_TexCoord).r;

  //return vec4((_Matrix_V * vec4(position, 1)).xyz, 1);

  vec3 cumulative = vec3(0);
  for (int i = 0; i < MAX_LIGHTS; i++) {
    cumulative += calculateLight(
        _Lights[i], position, normal, albedo, specular, shininess);
  }

  return vec4(cumulative, alpha);
}

