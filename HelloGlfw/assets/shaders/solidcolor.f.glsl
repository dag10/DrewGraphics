#version 330 core

uniform vec3 Albedo;

out vec4 FragColor;

void main() {
  FragColor = vec4(Albedo, 255);
}

