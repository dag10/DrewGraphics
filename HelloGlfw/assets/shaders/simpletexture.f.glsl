uniform sampler2D MainTex;

in vec2 v_TexCoord;

vec4 frag() {
  return texture(MainTex, v_TexCoord);
}

