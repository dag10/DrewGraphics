#include "SphereIntersectionShared.hlsl"

// Clips fragments to be within a the unit sphere of an orthographic projection.
float4 main(VertexToPixel input) : SV_TARGET{
  float3 uniformPos =
      (input.position.xyz / input.position.w) * float3(1, 1, 2) -
      float3(0, 0, 1);
  float dist = length(uniformPos);
  clip(1 - dist);
  return float4(1, 1, 1, 1);
}
