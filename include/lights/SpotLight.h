//
//  lights/SpotLight.h
//
#pragma once

#include <Light.h>

#include <glm/vec3.hpp>

namespace dg {

  class SpotLight : public Light {

    public:

      // Attenuation properties.
      float constant = 1.0f;
      float linear = 0.14f;
      float quadratic = 0.07f;

      // Spot light cone properties.
      glm::vec3 direction = glm::vec3(0, -1, 0);
      float cutoff = glm::radians(30.f); // Radians of light cone.
      float feather = glm::radians(5.f); // Radians of cone edge fading.

      virtual void SetMaterialProperties(int index, Material& material) const;

      SpotLight();
      SpotLight(glm::vec3 color, float ambient, float diffuse, float specular);
      SpotLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
      SpotLight(
          glm::vec3 direction, float cutoff, glm::vec3 color, float ambient,
          float diffuse, float specular);
      SpotLight(
          glm::vec3 direction, float cutoff, glm::vec3 ambient,
          glm::vec3 diffuse, glm::vec3 specular);

  }; // class SpotLight

} // namespace dg
