#include "includes\VertexShaderInput.hlsl"

cbuffer Camera : register(b0) {
  matrix _Matrix_M;
  matrix _Matrix_MVP;
  matrix _Matrix_Normal;
};

cbuffer ScreenQuadProperties : register(b2) {
  float2 _Scale;
  float2 _Offset;
}

struct VertexToPixel {
  float4 position : SV_POSITION;
  float2 texCoord : TEXCOORD;
};

VertexToPixel main(VertexShaderInput input) {
  VertexToPixel output;

  output.texCoord = input.texCoord;
  output.position = float4(input.position.xy * _Scale + _Offset, 0, 1);

  return output;
};
