//
//  lights/PointLight.h
//
#pragma once

#include <Light.h>

#include <glm/vec3.hpp>

namespace dg {

  class PointLight : public Light {

    public:

      // Attenuation properties.
      float constant = 1.0f;
      float linear = 0.14f;
      float quadratic = 0.07f;

      virtual void SetMaterialProperties(int index, Material& material) const;

      PointLight();
      PointLight(glm::vec3 color, float ambient, float diffuse, float specular);
      PointLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);

  }; // class PointLight

} // namespace dg
