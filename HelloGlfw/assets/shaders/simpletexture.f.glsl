uniform sampler2D MainTex;
uniform vec2 UVScale;

in vec2 v_TexCoord;

vec4 frag() {
  return vec4(CalculateLight(), 1.0) * texture(MainTex, v_TexCoord * UVScale);
}

