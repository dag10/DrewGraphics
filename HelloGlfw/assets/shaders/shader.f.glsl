#version 330 core

uniform sampler2D MainTex;

in vec3 varying_Color;
in vec2 varying_TexCoord;

out vec4 FragColor;

void main() {
  FragColor = texture(MainTex, varying_TexCoord) * vec4(varying_Color, 1.0);
}

