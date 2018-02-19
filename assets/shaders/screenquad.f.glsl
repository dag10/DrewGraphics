uniform sampler2D _Texture;
uniform bool _UseTexture;
uniform vec3 _Color;

in vec2 v_TexCoord;

vec4 frag() {
  if (_UseTexture) {
    return vec4(texture(_Texture, v_TexCoord));
  } else {
    return vec4(_Color, 1);
  }
}

