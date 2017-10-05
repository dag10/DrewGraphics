// This file is prepended to all fragment shaders.

#version 330 core

struct Light {
  vec3 position;

  // Illumination properties.
  vec3 diffuse;
  vec3 ambient;
  vec3 specular;

  // Attenuation properties.
  float constant;
  float linear;
  float quadratic;
};

in vec4 v_ScenePos;
in vec3 v_Normal;

out vec4 FragColor;

uniform Light _Light;

uniform vec3 _CameraPosition;

#line 1 1
