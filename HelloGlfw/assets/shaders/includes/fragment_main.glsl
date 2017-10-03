// This file is linked into every fragment shader.
// All fragment shaders should implement frag(), not main().

// If we're rendering this model through a portal, this mat4 is the
// inverse of the transform of the output portal-front in scene space.
// This is used to transform a fragment's position from scene-space to
// portal-space to determine which side of the portal we're on for culling.
uniform mat4 _InvPortal;

// Uniforms for phong lighting.
uniform bool _Lit;
uniform vec3 _LightColor;
uniform vec3 _LightPosition;
uniform vec3 _CameraPosition;
uniform float _AmbientStrength;
uniform float _DiffuseStrength;
uniform float _SpecularStrength;

vec4 frag();

void main() {
  if (_InvPortal[3][3] == 1 && (_InvPortal * v_ScenePos).z < 0) {
    discard;
  }

  FragColor = frag();
}

vec3 CalculateLight() {
  if (!_Lit) {
    return vec3(1);
  }

  vec3 ambient = _AmbientStrength * _LightColor;

  vec3 normal = normalize(v_Normal);
  vec3 lightDir = normalize(_LightPosition - v_ScenePos.xyz); 
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 diffuse = _DiffuseStrength * diff * _LightColor;

  vec3 viewDir = normalize(_CameraPosition - v_ScenePos.xyz);
  vec3 reflectDir = reflect(-lightDir, normal);
  float specularAmount = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  vec3 specular = _SpecularStrength * specularAmount * _LightColor;

  return (specular + diffuse + ambient);
}

