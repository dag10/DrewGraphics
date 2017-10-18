//
//  Light.h
//
#pragma once

#include <SceneObject.h>

#include <glm/vec3.hpp>

namespace dg {

  enum LightType {
    NullLightType        = 0,
    PointLightType       = 1,
    SpotLightType        = 2,
    DirectionalLightType = 3,
  };

  class Material;

  class Light : public SceneObject {

    public:

      static const int MAX_LIGHTS;

      // Illumination properties.
      glm::vec3 ambient;
      glm::vec3 diffuse;
      glm::vec3 specular;

      virtual void SetMaterialProperties(int index, Material& material) const;
      static void ClearMaterialProperties(int index, Material& material);

    protected:

      static const char *LIGHTS_ARRAY_NAME;

      Light() = default;
      Light(glm::vec3 color, float ambient, float diffuse, float specular);
      Light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);

      static const std::string LightProperty(
          int index, const std::string& property);

      LightType type = NullLightType;

  }; // class Light

} // namespace dg
