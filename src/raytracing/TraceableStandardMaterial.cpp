//
//  raytracing/TraceableStandardMaterial.cpp
//

#include <raytracing/TraceableStandardMaterial.h>
#include <raytracing/Rays.h>

dg::TraceableStandardMaterial dg::TraceableStandardMaterial::WithColor(
    glm::vec3 color) {
  TraceableStandardMaterial material;
  material.SetDiffuse(color);
  return material;
}

dg::TraceableStandardMaterial dg::TraceableStandardMaterial::WithColor(
    glm::vec4 color) {
  TraceableStandardMaterial material;
  material.SetDiffuse({ color.x, color.y, color.z });
  return material;
}

dg::TraceableStandardMaterial dg::TraceableStandardMaterial::WithTexture(
    std::shared_ptr<Texture> texture) {
  TraceableStandardMaterial material;
  material.SetDiffuse(texture);
  return material;
}

dg::TraceableStandardMaterial::TraceableStandardMaterial()
    : StandardMaterial() {}

dg::TraceableStandardMaterial::TraceableStandardMaterial(
    TraceableStandardMaterial &other)
    : StandardMaterial(other) {}

dg::TraceableStandardMaterial::TraceableStandardMaterial(TraceableStandardMaterial&& other) {
  *this = std::move(other);
}

dg::TraceableStandardMaterial& dg::TraceableStandardMaterial::operator=(TraceableStandardMaterial& other) {
  *this = TraceableStandardMaterial(other);
  return *this;
}

dg::TraceableStandardMaterial& dg::TraceableStandardMaterial::operator=(TraceableStandardMaterial&& other) {
  swap(*this, other);
  return *this;
}

void dg::swap(TraceableStandardMaterial& first, TraceableStandardMaterial& second) {
  using std::swap;
  swap((Material&)first, (Material&)second);
}

glm::vec3 dg::TraceableStandardMaterial::CalculateLight(
    const Light::ShaderData &light, glm::vec3 normal, glm::vec3 diffuseColor,
    glm::vec3 specularColor, glm::vec3 scenePos, glm::vec3 cameraPos) const {

  float shininess = GetRequiredProperty("_Material.shininess")->value._float;

  if (light.type == Light::LightType::NONE) {
    return glm::vec3(0);
  }

  // Ambient
  glm::vec3 ambient = light.ambient * diffuseColor;

  // Diffuse
  glm::vec3 norm = normalize(normal);
  glm::vec3 lightDir;
  if (light.type == Light::LightType::POINT || light.type == Light::LightType::SPOT) {
    lightDir = normalize(light.position - scenePos);
  } else if (light.type == Light::LightType::DIRECTIONAL) {
    lightDir = -light.direction;
  }
  float diff = std::max(glm::dot(norm, lightDir), 0.0f);
  glm::vec3 diffuse = light.diffuse * diff * diffuseColor;

  // Specular
  glm::vec3 viewDir = glm::normalize(cameraPos - scenePos);
  glm::vec3 reflectDir = glm::reflect(-lightDir, norm);
  float spec = pow(std::max(glm::dot(viewDir, reflectDir), 0.0f), shininess);
  glm::vec3 specular = light.specular * spec * specularColor;

  // Attenuation
  if (light.type == Light::LightType::POINT ||
      light.type == Light::LightType::SPOT) {
    float distance = glm::length(light.position - scenePos);
    float attenuation =
        1.0 / (light.constantCoeff + light.linearCoeff * distance +
               light.quadraticCoeff * (distance * distance));
    diffuse *= attenuation;
    ambient *= attenuation;
    specular *= attenuation;
  }

  // Spot light cutoff
  //if (light.type == Light::LightType::SPOT) {
    //float theta = glm::dot(lightDir, glm::normalize(-light.direction));
    //float epsilon = light.outerCutoff - light.innerCutoff;
    //float intensity = clamp((theta - cos(light.innerCutoff)) / epsilon, 0.0, 1.0);
    //diffuse *= intensity;
    //specular *= intensity;
  //}

	return specular + diffuse + ambient;
}

glm::vec3 dg::TraceableStandardMaterial::Shade(const RayResult& rayres) const {
  glm::vec3 diffuseColor =
      GetRequiredProperty("_Material.diffuse")->value._vec3;
  glm::vec3 specularColor =
      GetRequiredProperty("_Material.specular")->value._vec3;
  glm::vec3 scenePos =
      rayres.ray.origin + rayres.ray.direction * rayres.distance;
  glm::vec3 cameraPos = GetRequiredProperty("_CameraPosition")->value._vec3;
  glm::vec3 normal = rayres.normal;

  glm::vec3 cumulative(0);
  for (auto& light : lights) {
    cumulative += CalculateLight(light.second, normal, diffuseColor,
                                 specularColor, scenePos, cameraPos);
  }

  return cumulative;

  //glm::vec3 norm = glm::normalize(normal);
  //glm::vec3 lightDir;
  // Directional light.
  //lightDir = -light.direction;
  //float diff = max(dot(norm, lightDir), 0.0);
  //glm::vec3 diffuse = light.diffuse * diff * diffuseColor;
}
