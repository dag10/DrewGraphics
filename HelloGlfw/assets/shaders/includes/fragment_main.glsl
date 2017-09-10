// This file is linked into every fragment shader.
// All fragment shaders should implement frag(), not main().

// If we're rendering this model through a portal, this mat4 is the
// inverse of the transform of the output portal-front in scene space.
// This is used to transform a fragment's position from scene-space to
// portal-space to determine which side of the portal we're on for culling.
uniform mat4 InvPortal;

// Uniforms for phong lighting.
uniform bool Lit;
uniform vec3 LightColor;
uniform vec3 LightPosition;
uniform vec3 CameraPosition;
uniform float AmbientStrength;
uniform float DiffuseStrength;
uniform float SpecularStrength;

vec4 frag();

void main() {
  if (InvPortal[3][3] == 1 && (InvPortal * v_ScenePos).z < 0) {
    discard;
  }

  FragColor = frag();
}

vec3 CalculateLight() {
  if (!Lit) {
    return vec3(1);
  }

  vec3 ambient = AmbientStrength * LightColor;

  vec3 normal = normalize(v_Normal);
  vec3 lightDir = normalize(LightPosition - v_ScenePos.xyz); 
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 diffuse = DiffuseStrength * diff * LightColor;

  vec3 viewDir = normalize(CameraPosition - v_ScenePos.xyz);
  vec3 reflectDir = reflect(-lightDir, normal);
  float specularAmount = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  vec3 specular = SpecularStrength * specularAmount * LightColor;

  return (specular + diffuse + ambient);
}

