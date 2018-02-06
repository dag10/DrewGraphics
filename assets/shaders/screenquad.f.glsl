uniform sampler2D _Texture;

in vec2 v_TexCoord;

vec4 frag() {
  return vec4(texture(_Texture, v_TexCoord));
}

