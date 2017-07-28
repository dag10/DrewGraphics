#version 110

uniform float elapsed_time;

attribute vec4 position;

varying vec2 texcoord;
varying float fade_factor;

void main()
{
    const float aspect = 3.0 / 4.0;
    
    const mat3 projection = mat3(
        vec3(aspect, 0.0, 0.0),
        vec3(   0.0, 1.0, 0.0),
        vec3(   0.0, 0.0, 0.5)
    );

    mat3 rotation = mat3(
        vec3(1.0,                0.0,                0.0),
        vec3(0.0,  cos(elapsed_time),  sin(elapsed_time)),
        vec3(0.0, -sin(elapsed_time),  cos(elapsed_time))
    );
    
    mat3 scale = mat3(
        vec3(1.0 / aspect, 0.0, 0.0),
        vec3(         0.0, 1.0, 0.0),
        vec3(         0.0, 0.0, 1.0)
    );
    
    vec3 projected_position = projection * rotation * scale * position.xyz;
    float perspective_factor = projected_position.z * 0.5 + 1.0;
    
    gl_Position = vec4(projected_position/perspective_factor, 1.0);
    texcoord = position.xy * vec2(0.5) + vec2(0.5);
    fade_factor = sin(elapsed_time * 2.0) * 0.5 + 0.5;
}
