cbuffer Camera : register(b0) {
  matrix _Matrix_M;
  matrix _Matrix_MVP;
  matrix _Matrix_Normal;
};

struct VertexShaderInput {
  float3 position : POSITION;
  float3 normal : NORMAL;
  float2 texCoord : TEXCOORD;
  float3 tangent : TANGENT;
};

struct VertexToPixel {
  float4 position : SV_POSITION;
  float4 scenePos : POSITION;
  float4 normal : NORMAL;
  float4 tangent : TANGENT;
  float4 bitangent : BITANGENT;
  float2 texCoord : TEXCOORD;
};

VertexToPixel main(VertexShaderInput input) {
  VertexToPixel output;

  float3 normal =
      normalize(mul(_Matrix_Normal, float4(input.normal, 0.0f)).xyz);
  float3 tangent =
      normalize(mul(_Matrix_Normal, float4(input.tangent, 0.0f)).xyz);
  float3 bitangent = -normalize(cross(normal, tangent));

  output.scenePos = mul(_Matrix_M, float4(input.position, 1.0f));
  output.position = mul(_Matrix_MVP, float4(input.position, 1.0f));
  output.texCoord = input.texCoord;
  output.normal = float4(normal, 0);
  output.tangent = float4(tangent, 0);
  output.bitangent = float4(bitangent, 0);

  return output;
};
