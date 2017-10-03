uniform vec3 _Albedo;

vec4 frag() {
  return vec4(_Albedo * CalculateLight(), 1.0);
}

