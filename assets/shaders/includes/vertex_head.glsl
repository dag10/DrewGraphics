// This file is prepended to all vertex shaders.

#version 330 core

uniform mat4 _Matrix_MVP;
uniform mat4 _Matrix_M;

uniform mat4 _Matrix_Normal;

uniform mat4 _Matrix_Shadow_VP;

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 in_TexCoord;
layout (location = 3) in vec3 in_Tangent;

#line 1 1
