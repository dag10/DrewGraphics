uniform sampler2D MainTex;
uniform vec2 UVScale;

in vec2 v_TexCoord;

vec4 frag() {
  return texture(MainTex, v_TexCoord * UVScale);
}

