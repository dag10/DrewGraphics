// This file is prepended to all shaders.

#version 330 core

uniform mat4 _Matrix_MVP;
uniform mat4 _Matrix_M;

uniform mat4 _Matrix_Normal;

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
};

// NOTE: Keep this consistent with MAX_LIGHTS in src/Light.cpp.
const int MAX_LIGHTS = 8;
uniform Light _Lights[MAX_LIGHTS];

uniform vec3 _CameraPosition;

#line 1 1
