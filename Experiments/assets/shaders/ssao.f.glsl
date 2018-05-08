uniform sampler2D _ViewPositionTexture;
uniform sampler2D _NormalTexture;
uniform sampler2D _NoiseTexture;

uniform vec3 _Samples[64];

in vec2 v_TexCoord;

const int kernelSize = 64;
const float radius = 0.5;
const float bias = 0.025;

vec4 frag() {
  vec3 normal_WS = texture(_NormalTexture, v_TexCoord).xyz;

  // If no geometry was rendered for this fragment, discard.
  if (length(normal_WS) == 0) {
    discard;
  }

  vec3 normal_VS = (_Matrix_V * vec4(normal_WS, 0)).xyz;

  vec2 noiseScale = _BufferDimensions / 4;

  vec3 position_VS = texture(_ViewPositionTexture, v_TexCoord).xyz;
  vec3 randomVec = texture(_NoiseTexture, v_TexCoord * noiseScale).xyz;

  vec3 tangent = normalize(randomVec - normal_VS * dot(randomVec, normal_VS));
  vec3 bitangent = cross(normal_VS, tangent);
  mat3 TBN = mat3(tangent, bitangent, normal_VS);

  float occlusion = 0;
  for (int i = 0; i < kernelSize; i++) {
    // Sample position.
    vec3 sample = TBN * _Samples[i];
    sample = position_VS + sample * radius;

    vec4 offset = vec4(sample, 1.0);
    offset = _Matrix_P * offset; // Transform from view to clip space.
    offset.xyz /= offset.w;
    offset.xyz = offset.xyz * 0.5 + 0.5; // Transform to range [0, 1]

    float sampleDepth = texture(_ViewPositionTexture, offset.xy).z;

    float rangeCheck = smoothstep(
        0.0, 1.0, radius / abs(position_VS.z - sampleDepth));
    occlusion += (sampleDepth >= sample.z + bias ? 1.0 : 0.0) * rangeCheck;
  }

  occlusion = 1.0 - (occlusion / kernelSize);

  return vec4(vec3(occlusion), 1);
}

