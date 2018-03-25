//
//  Material.h
//
#pragma once

#include <memory>
#include <unordered_map>
#include "dg/Lights.h"
#include "dg/RasterizerState.h"
#include "dg/Shader.h"
#include "dg/Texture.h"

namespace dg {

  enum class MaterialPropertyType {
    NONE,

    BOOL,
    INT,
    FLOAT,
    VEC2,
    VEC3,
    VEC4,
    MAT4X4,
    TEXTURE,
  };

  union MaterialPropertyValue {
    bool _bool;
    int _int;
    float _float;
    glm::vec2 _vec2;
    glm::vec3 _vec3;
    glm::vec4 _vec4;
    glm::mat4x4 _mat4x4;

    MaterialPropertyValue() {
      memset(this, 0, sizeof(MaterialPropertyValue));
    }
  };

  struct MaterialProperty {
    MaterialPropertyType type = MaterialPropertyType::NONE;
    MaterialPropertyValue value;
    std::shared_ptr<Texture> texture = nullptr;
    int texUnitHint = -1;
  };

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
      void SetProperty(const std::string& name, glm::mat4x4 value);
      void SetProperty(
          const std::string& name, std::shared_ptr<Texture> value);
      void SetProperty(
          const std::string& name, std::shared_ptr<Texture> value,
          int texUnitHint);

      void ClearProperty(const std::string& name);

      void SendCameraPosition(glm::vec3 position);
      void SendMatrixMVP(glm::mat4x4 mvp);
      void SendMatrixM(glm::mat4x4 m);
      void SendMatrixNormal(glm::mat4x4 normal);
      void SendLights(const Light::ShaderData(&lights)[Light::MAX_LIGHTS]);

      // Portal world-to-local transform, for back-of-portal fragment culling.
      // Set this if we're currently rendering "through" a portal, and set
      // to zeros if we're not rendering through a portal.
      void SetInvPortal(glm::mat4x4 invPortal);

      void Use() const;

      RasterizerState rasterizerOverride;

    protected:

      static const std::string LightProperty(
          int index, const std::string& property);

#if defined(_OPENGL)
      void SendLight(int index, const Light::ShaderData& data);
      void ClearLights();
      void ClearLight(int index);
#endif

    private:

      std::unordered_map<std::string, MaterialProperty> properties;
      unsigned int highestTexUnitHint = 0;

  }; // class Material

} // namespace dg
