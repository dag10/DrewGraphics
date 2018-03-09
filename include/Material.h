//
//  Material.h
//
#pragma once

#include <memory>
#include <unordered_map>
#include <Shader.h>
#include <Texture.h>
#include <Light.h>

namespace dg {

  // Same as ShaderProperty, except it can own the texture.
  struct MaterialProperty {
    ShaderPropertyType type = PROPERTY_NULL;
    ShaderPropertyValue value;
    std::shared_ptr<Texture> texture = nullptr;
    int texUnitHint = -1;
  };

  class Material {

    public:

      Material() = default;
      virtual ~Material() {};

      Material(Material& other);
      Material(Material&& other);
      Material& operator=(Material& other);
      Material& operator=(Material&& other);
      friend void swap(Material& first, Material& second); // nothrow

      std::shared_ptr<Shader> shader = nullptr;

      void SetProperty(const std::string& name, bool value);
      void SetProperty(const std::string& name, int value);
      void SetProperty(const std::string& name, float value);
      void SetProperty(const std::string& name, glm::vec2 value);
      void SetProperty(const std::string& name, glm::vec3 value);
      void SetProperty(const std::string& name, glm::vec4 value);
      void SetProperty(const std::string& name, glm::mat2x2 value);
      void SetProperty(const std::string& name, glm::mat3x3 value);
      void SetProperty(const std::string& name, glm::mat4x4 value);
      void SetProperty(
          const std::string& name, std::shared_ptr<Texture> value);
      void SetProperty(
          const std::string& name, std::shared_ptr<Texture> value,
          int texUnitHint);

      void ClearProperty(const std::string& name);

      void SetCameraPosition(glm::vec3 position);
      void SetMatrixMVP(glm::mat4x4 mvp);
      void SetMatrixM(glm::mat4x4 m);
      void SetMatrixNormal(glm::mat3x3 normal);
      void SetLight(int index, const Light& light);
      void ClearLights();
      void ClearLight(int index);

      // Portal world-to-local transform, for back-of-portal fragment culling.
      // Set this if we're currently rendering "through" a portal, and set
      // to zeros if we're not rendering through a portal.
      void SetInvPortal(glm::mat4x4 invPortal);

      void Use() const;

    private:

      std::unordered_map<std::string, MaterialProperty> properties;
      unsigned int highestTexUnitHint = 0;

  }; // class Material

} // namespace dg
