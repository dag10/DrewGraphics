struct Material {
  bool lit;

  bool useDiffuseMap;
  sampler2D diffuseMap;
  vec3 diffuse;

  bool useSpecularMap;
  sampler2D specularMap;
  vec3 specular;

  float shininess;
};

uniform Material _Material;
uniform vec2 _UVScale;

in vec2 v_TexCoord;

vec4 frag() {
  vec2 texCoord = v_TexCoord * _UVScale;

  vec3 diffuseColor = _Material.useDiffuseMap
                    ? texture(_Material.diffuseMap, texCoord).rgb
                    : vec3(_Material.diffuse);

  if (!_Material.lit) {
    return vec4(diffuseColor, 1.0);
  }

  vec3 specularColor = _Material.useSpecularMap
                     ? texture(_Material.specularMap, texCoord).rgb
                     : vec3(_Material.specular);

  // Ambient
  vec3 ambient = _Light.ambient * diffuseColor;

  // Diffuse
  vec3 norm = normalize(v_Normal);
  vec3 lightDir = normalize(_Light.position - v_ScenePos.xyz); 
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = _Light.diffuse * diff * diffuseColor;

  // Specular
  vec3 viewDir = normalize(_CameraPosition - v_ScenePos.xyz);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), _Material.shininess);
  vec3 specular = _Light.specular * spec * specularColor;
  
  // Attenuation
  float distance = length(_Light.position - v_ScenePos.xyz);
  float attenuation = 1.0 / (_Light.constant + _Light.linear * distance +
                      _Light.quadratic * (distance * distance));
  diffuse *= attenuation;
  ambient *= attenuation;
  specular *= attenuation;

  return vec4(specular + diffuse + ambient, 1.0);
}

