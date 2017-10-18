//
//  lights/SpotLight.cpp
//

#include <lights/SpotLight.h>
#include <Material.h>

dg::SpotLight::SpotLight() : Light() {};

dg::SpotLight::SpotLight(
    glm::vec3 color, float ambient, float diffuse, float specular)
  : Light(color * ambient, color * diffuse, color * specular) {
  type = SpotLightType;
}

dg::SpotLight::SpotLight(
    glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
  : Light(ambient, diffuse, specular) {
  type = SpotLightType;
}

dg::SpotLight::SpotLight(
    glm::vec3 direction, float cutoff, glm::vec3 color, float ambient,
    float diffuse, float specular)
  : direction(direction), cutoff(cutoff),
    Light(color * ambient, color * diffuse, color * specular) {
  type = SpotLightType;
}

dg::SpotLight::SpotLight(
    glm::vec3 direction, float cutoff, glm::vec3 ambient, glm::vec3 diffuse,
    glm::vec3 specular)
  : direction(direction), cutoff(cutoff), Light(ambient, diffuse, specular) {
  type = SpotLightType;
}

void dg::SpotLight::SetMaterialProperties(
    int index, Material& material) const {
  Light::SetMaterialProperties(index, material);

  Transform xf = SceneSpace();
  material.SetProperty(
      LightProperty(index, "position"), SceneSpace().translation);
  material.SetProperty(LightProperty(index, "constant"), constant);
  material.SetProperty(LightProperty(index, "linear"), linear);
  material.SetProperty(LightProperty(index, "quadratic"), quadratic);
  material.SetProperty(
      LightProperty(index, "direction"), xf.rotation * direction);
  material.SetProperty(
      LightProperty(index, "innerCutoff"), cutoff - (feather / 2.f));
  material.SetProperty(
      LightProperty(index, "outerCutoff"), cutoff + (feather / 2.f));
}

