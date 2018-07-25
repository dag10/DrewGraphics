#version 330 core

uniform vec2 _UVScale;

in GlobalVertexData {
  vec4 v_ScenePos;
  vec3 v_Normal;
  mat3 v_TBN; // The tangent space basis vectors in scene space.
} g_vs_in;

in VertexData {
  vec2 v_TexCoord;
} vs_in;

layout (location = 0) out vec4 FragColor;

void main() {
  FragColor = vec4(vs_in.v_TexCoord, 0, 1);
}


