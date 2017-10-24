// This file is linked into every vertex shader.
// All vertex shaders should implement vert(), not main().

out vec4 v_ScenePos;
out vec3 v_Normal;
out mat3 v_TBN; // The tangent space basis vectors in scene space.

vec4 vert();

void main() {
  v_ScenePos = _Matrix_M * vec4(in_Position, 1.0);
  v_Normal = normalize(_Matrix_Normal * in_Normal);
  vec3 T = normalize(_Matrix_Normal * in_Tangent);
  // Bitangent is negative because OpenGL's Y coordinate for images is reversed.
  vec3 B = -normalize(cross(v_Normal, T));
  v_TBN = mat3(T, B, v_Normal);

  gl_Position = vert();
}

