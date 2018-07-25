// This file is prepended to all shaders.

#define LIGHT_TYPE_NULL        0
#define LIGHT_TYPE_POINT       1
#define LIGHT_TYPE_SPOT        2
#define LIGHT_TYPE_DIRECTIONAL 3

struct Light {
  // Type of light. Allowed values are those defined above.
	int type;

  // Light color properties.
  vec3 diffuse;
  vec3 ambient;
  vec3 specular;

  // Point light position.
  vec3 position;

  // Directional and spot light direction.
  vec3 direction;

  // Spot light cutoff angle.
  float innerCutoff;
  float outerCutoff;

  // Point light attenuation properties.
  float constantCoeff;
  float linearCoeff;
  float quadraticCoeff;

  // Shadow.
  int hasShadow;
  mat4 lightTransform;
};

uniform vec2 _BufferDimensions;
uniform vec2 _UVScale;

// NOTE: Keep this consistent with MAX_LIGHTS in include/dg/Lights.h.
const int MAX_LIGHTS = 8;
uniform Light _Lights[MAX_LIGHTS];

uniform vec3 _CameraPosition;

uniform mat4 _Matrix_MVP;
uniform mat4 _Matrix_M;
uniform mat4 _Matrix_V;
uniform mat4 _Matrix_P;
uniform mat4 _Matrix_Normal;
