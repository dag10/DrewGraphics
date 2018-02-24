#define MAX_LIGHTS 4

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
  float3 specular;
  float shininess;
  float2 uvScale;
};

struct VertexToPixel {
  float4 position : SV_POSITION;
  float4 scenePos : POSITION;
  float4 normal : NORMAL;
  float2 texCoord : TEXCOORD;
};

float3 calculateLight(Light light, VertexToPixel input) {
  if (light.type == LIGHT_TYPE_NULL) {
    return float3(0, 0, 0);
  }

  // Ambient
  float3 ambientComponent = light.ambient * diffuse;

  // Diffuse
  float3 norm = normalize(input.normal);
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
  if (input.position.z < 0.5) {
    return float4(0, 0, 0, 1);
  }
  return float4(diffuse, 1.0);

  //if (!lit) {
  //  return float4(diffuse, 1.0);
  //}

  //float3 cumulative = float3(0, 0, 0);
  //for (int i = 0; i < MAX_LIGHTS; i++) {
  //  cumulative += calculateLight(lights[i], input);
  //}

  //return float4(cumulative, 1.0);
}
