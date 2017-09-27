//
//  Material.h
//
#pragma once

#include <memory>
#include "Shader.h"
#include "Texture.h"

namespace dg {

  class Material {

    public:

      Material() = default;

      Material(Material& other);
      Material(Material&& other);
      Material& operator=(Material& other);
      Material& operator=(Material&& other);
      friend void swap(Material& first, Material& second); // nothrow

      std::shared_ptr<Shader> shader = nullptr;

      // TODO: Don't hard-code these into the model. Use a vector of shader
      //       properties instead.
      std::shared_ptr<Texture> texture = nullptr;
      glm::vec2 uvScale = glm::vec2(1);
      glm::mat4x4 invPortal = glm::mat4x4(0);

      bool lit = false;
      glm::vec3 albedo = glm::vec3(1);
      glm::vec3 lightColor = glm::vec3(0);
      float ambientStrength = 0;
      float diffuseStrength = 0;
      float specularStrength = 0;

      void Use() const;

  }; // class Material

} // namespace dg
