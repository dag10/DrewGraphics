uniform bool _AlbedoSampler;
uniform vec4 _Albedo;
uniform sampler2D _MainTex;

uniform vec2 _UVScale;

in vec2 v_TexCoord;

vec4 frag() {
  if (_AlbedoSampler) {
    return vec4(
        CalculateLight(), 1.0) * texture(_MainTex, v_TexCoord * _UVScale);
  } else {
    return _Albedo * vec4(CalculateLight(), 1.0);
  }
}

