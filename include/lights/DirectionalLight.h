//
//  lights/DirectionalLight.h
//
#pragma once

#include <Light.h>

#include <glm/vec3.hpp>

namespace dg {

  class DirectionalLight : public Light {

    public:

      glm::vec3 direction = glm::vec3(0, -1, 0);

      virtual void SetMaterialProperties(Material& material) const;

      DirectionalLight();
      DirectionalLight(
          glm::vec3 color, float ambient, float diffuse, float specular);
      DirectionalLight(
          glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
      DirectionalLight(
          glm::vec3 direction, glm::vec3 color, float ambient, float diffuse,
          float specular);
      DirectionalLight(
          glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse,
          glm::vec3 specular);

  }; // class DirectionalLight

} // namespace dg
