//
//  lights/PointLight.cpp
//

#include <lights/PointLight.h>
#include <Material.h>

dg::PointLight::PointLight() : Light() {};

dg::PointLight::PointLight(
    glm::vec3 color, float ambient, float diffuse, float specular)
  : Light(color * ambient, color * diffuse, color * specular) {
  type = PointLightType;
}

dg::PointLight::PointLight(
    glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
  : Light(ambient, diffuse, specular) {
  type = PointLightType;
}

void dg::PointLight::SetMaterialProperties(Material& material) const {
  Light::SetMaterialProperties(material);
  material.SetProperty("_Light.position", SceneSpace().translation);
  material.SetProperty("_Light.constant", constant);
  material.SetProperty("_Light.linear", linear);
  material.SetProperty("_Light.quadratic", quadratic);
}

