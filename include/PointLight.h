//
//  PointLight.h
//
#pragma once

#include <SceneObject.h>

#include <glm/vec3.hpp>

namespace dg {

  class PointLight : public SceneObject {

    public:

      // Illumination properties.
      glm::vec3 ambient;
      glm::vec3 diffuse;
      glm::vec3 specular;

      // Attenuation properties.
      float constant = 1.0f;
      float linear = 0.14f;
      float quadratic = 0.07f;

      PointLight();
      PointLight(glm::vec3 color, float ambient, float diffuse, float specular);
      PointLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);

  }; // class PointLight

} // namespace dg
