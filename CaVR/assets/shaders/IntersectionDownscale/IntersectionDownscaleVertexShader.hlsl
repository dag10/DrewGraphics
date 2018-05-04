#include "../../../../Engine/assets/shaders/includes/VertexShaderInput.hlsl"
#include "IntersectionDownscaleShared.hlsli"

VertexToPixel main(VertexShaderInput input) {
  VertexToPixel output;
  output.position = float4(input.position.xy * 2, 0, 1);
  return output;
};
