#include "CaveShared.hlsli"

#define MAX_LIGHTS 8

#define LIGHT_TYPE_NULL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2
#define LIGHT_TYPE_DIRECTIONAL 3

struct Light {
  float3 diffuse;
  int type;

  float3 ambient;
  float innerCutoff;

  float3 specular;
  float outerCutoff;

  float3 position;
  float constantCoeff;

  float3 direction;
  float linearCoeff;

  float quadraticCoeff;

  int hasShadow;

  float2 _padding;

  matrix lightTransform;
};

cbuffer Lights : register(b0) { Light lights[MAX_LIGHTS]; }

cbuffer Camera : register(b1) { float3 _CameraPosition; }

cbuffer Material : register(b2) {
  int lit;
  float4 diffuse;
  bool useDiffuseMap;
  float3 specular;
  bool useSpecularMap;
  float2 uvScale;
  float shininess;
  bool useNormalMap;
};

Texture2D diffuseTexture : register(t0);
SamplerState diffuseTextureSampler : register(s0);
Texture2D specularTexture : register(t1);
SamplerState specularTextureSampler : register(s1);
Texture2D normalTexture : register(t2);
SamplerState normalTextureSampler : register(s2);

float3 calculateLight(Light light, VertexToPixel input, float3 normal,
                      float3 diffuse, float3 specular) {
  if (light.type == LIGHT_TYPE_NULL) {
    return float3(0, 0, 0);
  }

  // Ambient
  float3 ambientComponent = light.ambient * diffuse;

  // Diffuse
  float3 norm = normalize(normal);
  float3 lightDir;
  if (light.type == LIGHT_TYPE_POINT || light.type == LIGHT_TYPE_SPOT) {
    lightDir = normalize(light.position - input.scenePos.xyz);
  } else if (light.type == LIGHT_TYPE_DIRECTIONAL) {
    lightDir = -light.direction;
  }
  float diff = max(dot(norm, lightDir), 0.0);
  float3 diffuseComponent = light.diffuse * diff * diffuse;

  // Specular
  float3 viewDir = normalize(_CameraPosition - input.scenePos.xyz);
  float3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
  float3 specularComponent = light.specular * spec * specular;

  // Attenuation
  if (light.type == LIGHT_TYPE_POINT || light.type == LIGHT_TYPE_SPOT) {
    float distance = length(light.position - input.scenePos.xyz);
    float attenuation =
        1.0 / (light.constantCoeff + light.linearCoeff * distance +
               light.quadraticCoeff * (distance * distance));
    diffuseComponent *= attenuation;
    ambientComponent *= attenuation;
    specularComponent *= attenuation;
  }

  // Spot light cutoff
  if (light.type == LIGHT_TYPE_SPOT) {
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.outerCutoff - light.innerCutoff;
    float intensity =
        clamp((theta - cos(light.innerCutoff)) / epsilon, 0.0, 1.0);
    diffuseComponent *= intensity;
    specularComponent *= intensity;
  }

  return specularComponent + diffuseComponent + ambientComponent;
}

float4 main(VertexToPixel input) : SV_TARGET {
  float2 texCoord = input.texCoord.xy * uvScale;

  float4 diffuseColor =
      //useDiffuseMap ? diffuseTexture.SampleLevel(diffuseTextureSampler, texCoord, 3.402823466e+38f)
      useDiffuseMap ? diffuseTexture.Sample(diffuseTextureSampler, texCoord)
                    : diffuse;

  if (!lit) {
    return diffuseColor;
  }

  float3 specularColor =
      useSpecularMap
          ? specularTexture.Sample(specularTextureSampler, texCoord).rgb
          : specular;

  float3 normal = input.normal.xyz;
  if (useNormalMap) {
    normal = normalize(
        normalTexture.Sample(normalTextureSampler, texCoord).rgb * 2.0 - 1.0);

    // Transform normal from tangent space (which is what the normal map is)
    // to world space by left-multiplying the world-space basis vectors of
    // this fragment's tangent space.
    normal = normalize(
        mul(normal, float3x4(input.tangent, input.bitangent, input.normal)).xyz);
  }

  float3 cumulative = float3(0, 0, 0);
  for (int i = 0; i < MAX_LIGHTS; i++) {
    cumulative += calculateLight(
      lights[i], input, normal, diffuseColor.rgb, specularColor);
  }

  // Fade out alpha based on distance from camera.
  float minCamDist = 5.0;
  float camDistance = distance(_CameraPosition, input.scenePos);
  camDistance = min(camDistance, minCamDist);
  diffuseColor.a = max(0, diffuseColor.a - (camDistance / minCamDist));

  // Fade out based on nearness to head.
  float nearFaceDist = 0.1;
  diffuseColor.a = max(0, diffuseColor.a - (nearFaceDist / camDistance));

  // Crash mark.
  float crashAmount = 0;
  if (length(crashPosition) > 0) {
    float maxCrashDist = 0.15;
    float crashDistance = distance(crashPosition, input.scenePos);
    crashAmount = 1.0 - (crashDistance / maxCrashDist);
    crashAmount = max(0, crashAmount);
    crashAmount *= sin(crashDistance * 25);
  }

  return lerp(float4(cumulative, diffuseColor.a), float4(1, 0, 0, 0.9),
              crashAmount);
}
