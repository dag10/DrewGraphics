in vec2 v_TexCoord;

vec4 frag() {
  return vec4(v_TexCoord.x, v_TexCoord.y, 0, 1);
}

