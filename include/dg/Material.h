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

  // Value for rendering order.
  enum class RenderQueue : int {
    Background  = 1000,
    Geometry    = 2000,
    Transparent = 3000,
    Overlay     = 4000,
  };

  using T = std::underlying_type_t<RenderQueue>;
  inline RenderQueue operator - (RenderQueue lhs, int rhs) {
    return (RenderQueue)(static_cast<T>(lhs) - rhs);
  };
  inline RenderQueue operator + (RenderQueue lhs, int rhs) {
    return (RenderQueue)(static_cast<T>(lhs) + rhs);
  };
  inline bool operator < (RenderQueue lhs, RenderQueue rhs) {
    return static_cast<T>(lhs) < static_cast<T>(rhs);
  };
  inline bool operator > (RenderQueue lhs, RenderQueue rhs) {
    return static_cast<T>(lhs) > static_cast<T>(rhs);
  };
  inline bool operator <= (RenderQueue lhs, RenderQueue rhs) {
    return static_cast<T>(lhs) <= static_cast<T>(rhs);
  };
  inline bool operator >= (RenderQueue lhs, RenderQueue rhs) {
    return static_cast<T>(lhs) >= static_cast<T>(rhs);
  };
  inline bool operator == (RenderQueue lhs, RenderQueue rhs) {
    return static_cast<T>(lhs) == static_cast<T>(rhs);
  };

  class Material {

    public:

    enum class PropertyType {
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

    union PropertyValue {
      bool _bool;
      int _int;
      float _float;
      glm::vec2 _vec2;
      glm::vec3 _vec3;
      glm::vec4 _vec4;
      glm::mat4x4 _mat4x4;

      PropertyValue() {
        memset(this, 0, sizeof(PropertyValue));
      }
    };

    struct Property {
      PropertyType type = PropertyType::NONE;
      PropertyValue value;
      std::shared_ptr<Texture> texture = nullptr;
      int texUnitHint = -1;
    };

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

      void SendShadowMap(std::shared_ptr<Texture> shadowMap);
      void SendLightTransform(glm::mat4x4 xfLight);

      // Portal world-to-local transform, for back-of-portal fragment culling.
      // Set this if we're currently rendering "through" a portal, and set
      // to zeros if we're not rendering through a portal.
      void SetInvPortal(glm::mat4x4 invPortal);

      void Use() const;

      RasterizerState rasterizerOverride;
      RenderQueue queue = RenderQueue::Geometry;

    protected:

      enum class TexUnitHints {
        SHADOWMAP = 0,

        END,
      };

      static const std::string LightProperty(
          int index, const std::string& property);

#if defined(_OPENGL)
      void SendLight(int index, const Light::ShaderData& data);
      void ClearLights();
      void ClearLight(int index);
#endif

    private:

      std::unordered_map<std::string, Property> properties;
      unsigned int highestTexUnitHint = 0;

  }; // class Material

} // namespace dg
