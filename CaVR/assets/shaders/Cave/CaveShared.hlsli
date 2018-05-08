struct VertexToPixel {
  float4 position : SV_POSITION;
  float4 scenePos : POSITION;
  float4 normal : NORMAL;
  float4 tangent : TANGENT;
  float4 bitangent : BITANGENT;
  float2 texCoord : TEXCOORD;
};

cbuffer Cave : register(b3) {
  bool isFront;
  float3 crashPosition;
}
