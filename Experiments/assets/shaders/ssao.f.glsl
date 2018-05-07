uniform sampler2D _PositionTexture;
uniform sampler2D _NormalTexture;
uniform sampler2D _NoiseTexture;

uniform mat4 _Matrix_V;

in vec2 v_TexCoord;

vec4 frag() {
  vec3 normal = texture(_NormalTexture, v_TexCoord).xyz;

  // If no geometry was rendered for this fragment, discard.
  if (length(normal) == 0) {
    discard;
  }

  vec3 position = texture(_PositionTexture, v_TexCoord).xyz;

  return vec4((_Matrix_V * vec4(position, 1)).xyz, 1);
  //return vec4(1, 0.5, 0.3, 1);
}

