#version 110

uniform float elapsed_time;

attribute vec4 position;

varying vec2 texcoord;
varying float fade_factor;

void main()
{
    gl_Position = position;
    texcoord = position.xy * vec2(0.5) + vec2(0.5);
    fade_factor = sin(elapsed_time * 2.0) * 0.5 + 0.5;
}
