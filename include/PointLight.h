//
//  PointLight.h
//
#pragma once

#include <SceneObject.h>

#include <glm/vec3.hpp>

namespace dg {

  class PointLight : public SceneObject {

    public:
      glm::vec3 ambient;
      glm::vec3 diffuse;
      glm::vec3 specular;

      PointLight() = default;
      PointLight(glm::vec3 color, float ambient, float diffuse, float specular);
      PointLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);

  }; // class PointLight

} // namespace dg
