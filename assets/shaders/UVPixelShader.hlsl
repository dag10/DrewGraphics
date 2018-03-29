struct VertexToPixel {
  float4 position : SV_POSITION;
  float2 texCoord : TEXCOORD;
};

float4 main(VertexToPixel input) : SV_TARGET {
  return float4(input.texCoord.xy, 0, 1);
}
