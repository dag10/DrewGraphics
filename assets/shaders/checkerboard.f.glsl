uniform vec2 _Size;
uniform vec3 _Colors[2];

in vec2 v_TexCoord;

vec4 frag() {
  vec2 coord = v_TexCoord * _Size;
  int parity = int(coord.x) % 2 ^ int(coord.y) % 2;
  return vec4(_Colors[parity], 1);
}

