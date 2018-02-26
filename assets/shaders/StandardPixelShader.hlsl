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
  float3 _padding;
};

cbuffer Lights : register(b0) { Light lights[MAX_LIGHTS]; }

cbuffer Camera : register(b1) { float3 _CameraPosition; }

cbuffer Material : register(b2) {
  int lit;
  float3 diffuse;
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

struct VertexToPixel {
  float4 position : SV_POSITION;
  float4 scenePos : POSITION;
  float4 normal : NORMAL;
  float4 tangent : TANGENT;
  float4 bitangent : BITANGENT;
  float2 texCoord : TEXCOORD;
};

float3 calculateLight(Light light, VertexToPixel input, float3 normal,
                      float3 diffuse, float3 specular) {
  // Ambient
  float3 ambientComponent = light.ambient * diffuse;

  // Diffuse
  float3 norm = normalize(normal);
  float3 lightDir = float3(0, -1, 0);
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

  float3 diffuseColor =
      useDiffuseMap ? diffuseTexture.Sample(diffuseTextureSampler, texCoord).rgb
                    : diffuse;

  if (!lit) {
    return float4(diffuseColor, 1);
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
    normal =
        normalize(mul(normal, float3x3(input.tangent.xyz, input.bitangent.xyz,
                                       input.normal.xyz)));
  }

  float3 cumulative = float3(0, 0, 0);
  for (int i = 0; i < MAX_LIGHTS; i++) {
    if (lights[i].type != LIGHT_TYPE_NULL) {
      cumulative +=
          calculateLight(lights[i], input, normal, diffuseColor, specularColor);
    }
  }

  return float4(cumulative, 1.0);
}
