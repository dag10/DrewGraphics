//
//  Light.cpp
//

#include <Light.h>
#include <Material.h>

dg::Light::Light(
    glm::vec3 color, float ambient, float diffuse, float specular)
  : Light(color * ambient, color * diffuse, color * specular) {}

dg::Light::Light(
    glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
  : ambient(ambient), diffuse(diffuse), specular(specular) {}

void dg::Light::SetMaterialProperties(Material& material) const {
  material.SetProperty("_Light.type", (int)type);
  material.SetProperty("_Light.ambient", ambient);
  material.SetProperty("_Light.diffuse", diffuse);
  material.SetProperty("_Light.specular", specular);
}

