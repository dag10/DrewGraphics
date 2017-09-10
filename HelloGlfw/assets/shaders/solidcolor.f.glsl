uniform vec3 Albedo;

vec4 frag() {
  return vec4(Albedo * CalculateLight(), 1.0);
}

