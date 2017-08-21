#version 330 core

in vec3 varying_Color;

out vec4 FragColor;

void main() {
  FragColor = vec4(varying_Color, 1.0);
}

