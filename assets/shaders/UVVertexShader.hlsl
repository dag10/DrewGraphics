#include "includes\VertexShaderInput.hlsl"

cbuffer Camera : register(b0) {
  matrix _Matrix_M;
  matrix _Matrix_MVP;
  matrix _Matrix_Normal;
};

struct VertexToPixel {
  float4 position : SV_POSITION;
  float2 texCoord : TEXCOORD;
};

VertexToPixel main(VertexShaderInput input) {
  VertexToPixel output;

  output.texCoord = input.texCoord;
  output.position = mul(_Matrix_MVP, float4(input.position, 1.0f));

  return output;
};
