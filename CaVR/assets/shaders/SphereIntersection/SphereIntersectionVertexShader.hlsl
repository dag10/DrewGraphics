#include "../../../../Engine/assets/shaders/includes/VertexShaderInput.hlsl"
#include "SphereIntersectionShared.hlsli"

cbuffer Camera : register(b0) {
  matrix _Matrix_M;
  matrix _Matrix_MVP;
  matrix _Matrix_Normal;
};

VertexToPixel main(VertexShaderInput input) {
  VertexToPixel output;
  output.svPosition = mul(_Matrix_MVP, float4(input.position, 1.0f));
  output.position = output.svPosition;
  return output;
};
