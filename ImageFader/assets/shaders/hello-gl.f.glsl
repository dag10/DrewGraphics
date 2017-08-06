#version 110

uniform sampler2D textures[2];
uniform bool invertTexture;

varying vec2 texcoord;
varying float fade_factor;

void main()
{
    float fade = fade_factor;

    if (invertTexture) {
        fade = 1.0 - fade_factor;
    }

    gl_FragColor = mix(
        texture2D(textures[0], texcoord),
        texture2D(textures[1], texcoord),
        fade
    );
}
