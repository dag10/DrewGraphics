//
//  Material.h
//
#pragma once

#include <memory>
#include <map>
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
          const std::string& name, std::shared_ptr<Texture>  value);

      void Use() const;

    private:

      std::map<std::string, ShaderProperty> properties;

  }; // class Material

} // namespace dg
