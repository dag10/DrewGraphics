// This file is prepended to all vertex shaders.

#version 330 core

uniform mat4 _Matrix_MVP;
uniform mat4 _Matrix_M;

uniform mat3 _Matrix_Normal;

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 in_TexCoord;

#line 1 1
