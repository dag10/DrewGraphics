//
//  lights/DirectionalLight.cpp
//

#include <lights/DirectionalLight.h>
#include <Material.h>

dg::DirectionalLight::DirectionalLight() : Light() {};

dg::DirectionalLight::DirectionalLight(
    glm::vec3 color, float ambient, float diffuse, float specular)
  : Light(color * ambient, color * diffuse, color * specular) {
  type = DirectionalLightType;
}

dg::DirectionalLight::DirectionalLight(
    glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
  : Light(ambient, diffuse, specular) {
  type = DirectionalLightType;
}

dg::DirectionalLight::DirectionalLight(
    glm::vec3 direction, glm::vec3 color, float ambient, float diffuse,
    float specular)
  : direction(direction), Light(
      color * ambient, color * diffuse, color * specular) {
  type = DirectionalLightType;
}

dg::DirectionalLight::DirectionalLight(
    glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse,
    glm::vec3 specular)
  : direction(direction), Light(ambient, diffuse, specular) {
  type = DirectionalLightType;
}

void dg::DirectionalLight::SetMaterialProperties(Material& material) const {
  Light::SetMaterialProperties(material);
  material.SetProperty("_Light.direction", direction);
}

