//
//  PointLight.cpp
//

#include <PointLight.h>

dg::PointLight::PointLight(
    glm::vec3 color, float ambient, float diffuse, float specular)
  : PointLight(color * ambient, color * diffuse, color * specular) {}

dg::PointLight::PointLight(
    glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
  : ambient(ambient), diffuse(diffuse), specular(specular) { }

