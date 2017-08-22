#version 330 core

uniform sampler2D MainTex;
uniform sampler2D SecondaryTex;

uniform float ELAPSED_TIME;

in vec3 varying_Color;
in vec2 varying_TexCoord;

out vec4 FragColor;

void main() {
  FragColor = mix(
      texture(MainTex, varying_TexCoord),
      texture(SecondaryTex, varying_TexCoord),
      0.5 + 0.2 * sin(ELAPSED_TIME * 4));
}

