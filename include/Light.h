//
//  Light.h
//
#pragma once

#include <SceneObject.h>

#include <glm/vec3.hpp>

namespace dg {

  enum LightType {
    UnknownLightType     = 0,
    PointLightType       = 1,
    DirectionalLightType = 2,
  };

  class Material;

  class Light : public SceneObject {

    public:

      // Illumination properties.
      glm::vec3 ambient;
      glm::vec3 diffuse;
      glm::vec3 specular;

      virtual void SetMaterialProperties(Material& material) const;

    protected:

      Light() = default;
      Light(glm::vec3 color, float ambient, float diffuse, float specular);
      Light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);

      LightType type = UnknownLightType;

  }; // class Light

} // namespace dg
