#version 330 core

uniform vec2 _UVScale;

uniform vec3 _CameraPosition;

uniform mat4 _Matrix_MVP;
uniform mat4 _Matrix_M;
uniform mat4 _Matrix_V;
uniform mat4 _Matrix_P;
uniform mat4 _Matrix_Normal;

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 in_TexCoord;
layout (location = 3) in vec3 in_Tangent;

out GlobalVertexData {
  vec4 v_ScenePos;
  vec3 v_Normal;
  mat3 v_TBN; // The tangent space basis vectors in scene space.
} g_vs_out;

out VertexData {
  vec2 v_TexCoord;
} vs_out;

void main() {
  g_vs_out.v_ScenePos = _Matrix_M * vec4(in_Position, 1.0);
  g_vs_out.v_Normal = normalize(_Matrix_Normal * vec4(in_Normal, 0)).xyz;
  vec3 T = normalize(_Matrix_Normal * vec4(in_Tangent, 0)).xyz;
  // Bitangent is negative because OpenGL's Y coordinate for images is reversed.
  vec3 B = -normalize(cross(g_vs_out.v_Normal, T));
  g_vs_out.v_TBN = mat3(T, B, g_vs_out.v_Normal);

  vs_out.v_TexCoord = _UVScale; // TODO TMP   
  gl_Position = _Matrix_MVP * vec4(in_Position, 1.0);
}

