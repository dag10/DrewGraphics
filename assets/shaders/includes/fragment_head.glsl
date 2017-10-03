// This file is prepended to all fragment shaders.

#version 330 core

in vec4 v_ScenePos;
in vec3 v_Normal;

out vec4 FragColor;

vec3 CalculateLight();

#line 1 1
