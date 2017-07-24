#version 110

uniform float elapsed_time;

attribute vec4 position;

varying vec2 texcoord;
varying float fade_factor;

void main()
{
    const float aspect = 3.0 / 4.0;
    
    const mat4 projection = mat4(
        vec4(aspect, 0.0, 0.0, 0.0),
        vec4(   0.0, 1.0, 0.0, 0.0),
        vec4(   0.0, 0.0, 0.5, 0.5),
        vec4(   0.0, 0.0, 0.0, 1.0)
    );

    mat4 rotation = mat4(
        vec4(1.0,                0.0,                0.0, 0.0),
        vec4(0.0,  cos(elapsed_time),  sin(elapsed_time), 0.0),
        vec4(0.0, -sin(elapsed_time),  cos(elapsed_time), 0.0),
        vec4(0.0,                0.0,                0.0, 1.0)
    );
    
    mat4 scale = mat4(
        vec4(1.0 / aspect, 0.0, 0.0, 0.0),
        vec4(         0.0, 1.0, 0.0, 0.0),
        vec4(         0.0, 0.0, 1.0, 0.0),
        vec4(         0.0, 0.0, 0.0, 1.0)
    );
    
    gl_Position = projection * rotation * scale * position;
    texcoord = position.xy * vec2(0.5) + vec2(0.5);
    fade_factor = sin(elapsed_time * 2.0) * 0.5 + 0.5;
}
