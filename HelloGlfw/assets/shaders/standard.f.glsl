uniform bool AlbedoSampler;
uniform vec4 Albedo;
uniform sampler2D MainTex;

uniform vec2 UVScale;

in vec2 v_TexCoord;

vec4 frag() {
  if (AlbedoSampler) {
    return vec4(CalculateLight(), 1.0) * texture(MainTex, v_TexCoord * UVScale);
  } else {
    return Albedo * vec4(CalculateLight(), 1.0);
  }
}

