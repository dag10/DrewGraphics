#version 330 core
#include "includes/shared_head.glsl"

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;
//layout (triangle_strip, max_vertices=3) out;

uniform mat4 _CubemapMatrices[6];

in VertexData {
  vec2 v_TexCoord;
} gs_in[];

in GlobalVertexData {
  vec4 v_ScenePos;
  vec3 v_Normal;
  mat3 v_TBN; // The tangent space basis vectors in scene space.
} g_gs_in[];

out VertexData {
  vec2 v_TexCoord;
} gs_out;

out GlobalVertexData {
  vec4 v_ScenePos;
  vec3 v_Normal;
  mat3 v_TBN; // The tangent space basis vectors in scene space.
} g_gs_out;

//out vec4 FragPos; // FragPos from GS (output per emitvertex)

void main() {

  for (int face = 0; face < 6; face++) {
    // Which face of the cubemap to render onto.
    gl_Layer = face;

    // For each triangle's vertex.
    for (int i = 0; i < 3; i++) {

      //vec4 FragPos = gl_in[i].gl_Position;

      //gl_Position = _CubemapMatrices[face] * FragPos;
      //gl_Position = _Matrix_P * _Matrix_V * (g_gs_in[i].v_ScenePos + vec4(face, 0, 0, 0));
      //gl_Position = _Matrix_P * _Matrix_V * g_gs_in[i].v_ScenePos;
      gl_Position = gl_in[i].gl_Position;

      gs_out.v_TexCoord = gs_in[i].v_TexCoord;
      //gs_out.v_TexCoord = vec2(0, 1);
      g_gs_out.v_ScenePos = g_gs_in[i].v_ScenePos;
      g_gs_out.v_Normal = g_gs_in[i].v_Normal;
      g_gs_out.v_TBN = g_gs_in[i].v_TBN;

      EmitVertex();
    }

    EndPrimitive();
  }

}
