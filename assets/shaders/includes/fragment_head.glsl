// This file is prepended to all fragment shaders.

#version 330 core

#define LIGHT_TYPE_NULL        0
#define LIGHT_TYPE_POINT       1
#define LIGHT_TYPE_DIRECTIONAL 2

struct Light {
  // Type of light. Allowed values are those defined above.
	int type;

  // Light color properties.
  vec3 diffuse;
  vec3 ambient;
  vec3 specular;

  // Point light position.
  vec3 position;

  // Directional light direction.
  vec3 direction;

  // Point light attenuation properties.
  float constant;
  float linear;
  float quadratic;
};

in vec4 v_ScenePos;
in vec3 v_Normal;

out vec4 FragColor;

const int MAX_LIGHTS = 2;
uniform Light _Lights[MAX_LIGHTS];

uniform vec3 _CameraPosition;

#line 1 1
