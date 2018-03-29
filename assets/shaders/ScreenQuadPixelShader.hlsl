cbuffer ScreenQuadProperties : register(b2) {
  bool _UseTexture;
  bool _RedChannelOnly;
  float3 _Color;
};

Texture2D quadTexture : register(t0);
SamplerState quadTextureSampler : register(s0);

struct VertexToPixel {
  float4 position : SV_POSITION;
  float2 texCoord : TEXCOORD;
};

float4 main(VertexToPixel input) : SV_TARGET {
  float4 color =
      _UseTexture ? quadTexture.Sample(quadTextureSampler, input.texCoord)
                  : float4(_Color, 1);
  return _RedChannelOnly ? color.rrra : color;
}
