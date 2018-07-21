// This file is prepended to all fragment shaders.

in GlobalVertexData {
  vec4 v_ScenePos;
  vec3 v_Normal;
  mat3 v_TBN; // The tangent space basis vectors in scene space.
} g_vs_in;

layout (location = 0) out vec4 FragColor;
