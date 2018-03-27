// This file is linked into every vertex shader.
// All vertex shaders should implement vert(), not main().

out vec4 v_ScenePos;
out vec3 v_Normal;
out mat3 v_TBN; // The tangent space basis vectors in scene space.

// Fragment position in the shadow-producing light's clip space.
out vec4 v_FragPosLightSpace;

vec4 vert();

void main() {
  mat4 lightTransform;
  for (int i = 0; i < MAX_LIGHTS; i++) {
    if (_Lights[i].type != LIGHT_TYPE_NULL && _Lights[i].hasShadow == 1) {
      lightTransform = _Lights[i].lightTransform;
    }
  }

  v_ScenePos = _Matrix_M * vec4(in_Position, 1.0);
  v_FragPosLightSpace = lightTransform * v_ScenePos;
  v_Normal = normalize(_Matrix_Normal * vec4(in_Normal, 0)).xyz;
  vec3 T = normalize(_Matrix_Normal * vec4(in_Tangent, 0)).xyz;
  // Bitangent is negative because OpenGL's Y coordinate for images is reversed.
  vec3 B = -normalize(cross(v_Normal, T));
  v_TBN = mat3(T, B, v_Normal);

  gl_Position = vert();
}

