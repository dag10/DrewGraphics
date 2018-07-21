// This file is linked into every fragment shader.
// All fragment shaders should implement frag(), not main().

vec4 frag();

void main() {
  FragColor = frag();
}

