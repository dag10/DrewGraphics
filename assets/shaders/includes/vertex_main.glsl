// This file is linked into every vertex shader.
// All vertex shaders should implement vert(), not main().

out vec4 v_ScenePos;
out vec3 v_Normal;
flat out mat3 v_TBN; // The tangent space basis vectors in scene space.

vec4 vert();

void main() {
  v_ScenePos = _Matrix_M * vec4(in_Position, 1.0);
  v_Normal = normalize(_Matrix_Normal * in_Normal);
  v_TBN = mat3(
      normalize(vec3(_Matrix_M * vec4(in_Tangent, 0))),
      normalize(vec3(_Matrix_M * vec4(in_Bitangent, 0))),
      normalize(vec3(_Matrix_M * vec4(in_Normal, 0))));

  gl_Position = vert();
}

