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

void dg::PointLight::SetMaterialProperties(
    int index, Material& material) const {
  Light::SetMaterialProperties(index, material);
  material.SetProperty(
      LightProperty(index, "position"), SceneSpace().translation);
  material.SetProperty(LightProperty(index, "constant"), constant);
  material.SetProperty(LightProperty(index, "linear"), linear);
  material.SetProperty(LightProperty(index, "quadratic"), quadratic);
}

