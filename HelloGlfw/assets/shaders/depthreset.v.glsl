#version 330 core

uniform mat4 MATRIX_MVP;

layout (location = 0) in vec3 in_Position;

void main() {
  // The Quad mesh goes from -0.5 to 0.5, so double the coordinates
  // to fill the normalized device coordinates.
  gl_Position = vec4(in_Position * 2.0, 1.0);

  // Depth is farthest possible.
  gl_Position.z = 1;
}

