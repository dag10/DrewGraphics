// This file is linked into every vertex shader.
// All vertex shaders should implement vert(), not main().

out vec4 v_ScenePos;
out vec3 v_Normal;

vec4 vert();

void main() {
  v_ScenePos = MATRIX_M * vec4(in_Position, 1.0);
  v_Normal = normalize(MATRIX_NORMAL * in_Normal);

  gl_Position = vert();
}

