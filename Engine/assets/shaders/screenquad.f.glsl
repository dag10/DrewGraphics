uniform sampler2D _Texture;
uniform bool _UseTexture;
uniform bool _RedChannelOnly;
uniform vec3 _Color;

in vec2 v_TexCoord;

vec4 frag() {
  vec4 color = _UseTexture
    ? vec4(texture(_Texture, v_TexCoord))
    : vec4(_Color, 1);
  return _RedChannelOnly ? color.rrra : color;
}

