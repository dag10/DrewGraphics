out vec2 v_TexCoord;

vec4 vert() {
  v_TexCoord = in_TexCoord;
  return vec4(in_Position * 2.0, 1.0);
}

