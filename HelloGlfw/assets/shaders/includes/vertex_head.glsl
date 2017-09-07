// This file is prepended to all vertex shaders.

#version 330 core

uniform mat4 MATRIX_MVP;
uniform mat4 MATRIX_M;

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec2 in_TexCoord;

#line 1 1
