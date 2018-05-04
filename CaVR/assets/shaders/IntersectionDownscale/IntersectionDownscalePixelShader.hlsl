#include "IntersectionDownscaleShared.hlsli"

Texture2D intersectionTexture : register(t0);
SamplerState intersectionTextureSampler : register(s0);

static const float FLOAT_MAX = 3.402823466e+38f;

float4 main(VertexToPixel input) : SV_TARGET {
  float3 avg =
      intersectionTexture
          .SampleLevel(intersectionTextureSampler, float2(0.5, 0.5), FLOAT_MAX)
          .rgb;
  return float4(ceil(avg), 1);
}
