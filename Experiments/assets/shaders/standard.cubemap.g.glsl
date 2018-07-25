#version 330 core

// This file is prepended to all shaders.

//#define LIGHT_TYPE_NULL        0
//#define LIGHT_TYPE_POINT       1
//#define LIGHT_TYPE_SPOT        2
//#define LIGHT_TYPE_DIRECTIONAL 3

//struct Light {
  //// Type of light. Allowed values are those defined above.
	//int type;

  //// Light color properties.
  //vec3 diffuse;
  //vec3 ambient;
  //vec3 specular;

  //// Point light position.
  //vec3 position;

  //// Directional and spot light direction.
  //vec3 direction;

  //// Spot light cutoff angle.
  //float innerCutoff;
  //float outerCutoff;

  //// Point light attenuation properties.
  //float constantCoeff;
  //float linearCoeff;
  //float quadraticCoeff;

  //// Shadow.
  //int hasShadow;
  //mat4 lightTransform;
//};

//uniform vec2 _BufferDimensions;
//uniform vec2 _UVScale;

// NOTE: Keep this consistent with MAX_LIGHTS in include/dg/Lights.h.
//const int MAX_LIGHTS = 8;
//uniform Light _Lights[MAX_LIGHTS];

//uniform vec3 _CameraPosition;

//uniform mat4 _Matrix_MVP;
//uniform mat4 _Matrix_M;
//uniform mat4 _Matrix_V;
uniform mat4 _Matrix_P;
//uniform mat4 _Matrix_Normal;

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;
//layout (triangle_strip, max_vertices=3) out;

//uniform mat4 _CubemapMatrices[6];

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
      //gl_Position = gl_in[i].gl_Position;
      gl_Position = g_gs_in[i].v_ScenePos;

      mat4 TMP = _Matrix_P;

      gs_out.v_TexCoord = gs_in[i].v_TexCoord;
      //gs_out.v_TexCoord = vec2(0, 1);
      //g_gs_out.v_ScenePos = g_gs_in[i].v_ScenePos;
      //g_gs_out.v_Normal = g_gs_in[i].v_Normal;
      //g_gs_out.v_TBN = g_gs_in[i].v_TBN;

      EmitVertex();
    }

    EndPrimitive();
  }

}
