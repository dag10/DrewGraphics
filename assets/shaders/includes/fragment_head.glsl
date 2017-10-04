// This file is prepended to all fragment shaders.

#version 330 core

in vec4 v_ScenePos;
in vec3 v_Normal;

out vec4 FragColor;

uniform vec3 _LightColor;
uniform vec3 _LightPosition;

uniform vec3 _CameraPosition;

#line 1 1
