// This file is linked into every vertex shader.
// All vertex shaders should implement vert(), not main().

out vec4 v_ScenePos;

vec4 vert();

void main() {
  v_ScenePos = MATRIX_M * vec4(in_Position, 1.0);

  gl_Position = vert();
}

