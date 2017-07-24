#version 110

uniform float elapsed_time;

attribute vec4 position;

varying vec2 texcoord;
varying float fade_factor;

void main()
{
    float aspect = 3.0 / 4.0;
    
    mat3 window_scale = mat3(
        vec3(aspect, 0.0, 0.0),
        vec3(   0.0, 1.0, 0.0),
        vec3(   0.0, 0.0, 1.0)
                             );
    
    mat3 object_scale = mat3(
        vec3(1.0 / aspect, 0.0, 0.0),
        vec3(         0.0, 1.0, 0.0),
        vec3(         0.0, 0.0, 1.0)
    );

    mat3 rotation = mat3(
        vec3( cos(elapsed_time),  sin(elapsed_time),  0.0),
        vec3(-sin(elapsed_time),  cos(elapsed_time),  0.0),
        vec3(               0.0,                0.0,  1.0)
    );
    
    gl_Position = vec4(window_scale * rotation * object_scale * position.xyz, 1.0);
    texcoord = position.xy * vec2(0.5) + vec2(0.5);
    fade_factor = sin(elapsed_time * 2.0) * 0.5 + 0.5;
}
