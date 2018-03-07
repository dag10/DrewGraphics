//
//  Lights.h
//

#pragma once

#include "dg/SceneObject.h"
#include <glm/vec3.hpp>

namespace dg {

  class Material;

  class Light : public SceneObject {

    public:

      // NOTE: Keep these values consistent with:
      //       -> assets/shaders/fragment_head.glsl
      //       -> assets/shaders/StandardPixelShader.hlsl.
      static const char *LIGHTS_ARRAY_NAME;
      static const int MAX_LIGHTS = 8;

      // NOTE: Keep this struct consistent with:
      //       -> assets/shaders/fragment_head.glsl
      //       -> assets/shaders/StandardPixelShader.hlsl.
      enum class LightType : uint32_t {
        NONE        = 0,
        POINT       = 1,
        SPOT        = 2,
        DIRECTIONAL = 3,
      };

      // Struct size must be a multiple of 16 bytes, and vectors cannot
      // cross 16-byte boundaries. Hence the confusing order and 3 bytes of
      // padding.
      //
      // NOTE: Keep this struct consistent with:
      //       -> assets/shaders/fragment_head.glsl
      //       -> assets/shaders/StandardPixelShader.hlsl.
      struct ShaderData {
        glm::vec3 diffuse;
        LightType type = LightType::NONE;
        glm::vec3 ambient;
        float innerCutoff;
        glm::vec3 specular;
        float outerCutoff;
        glm::vec3 position;
        float constantCoeff = 1.0f;
        glm::vec3 direction = glm::vec3(0, -1, 0);
        float linearCoeff = 0.14f;
        float quadraticCoeff = 0.07f;
        glm::vec3 _padding;
      };

      void SetAmbient(const glm::vec3& ambient);
      void SetDiffuse(const glm::vec3& diffuse);
      void SetSpecular(const glm::vec3& specular);

      glm::vec3 GetAmbient() const;
      glm::vec3 GetDiffuse() const;
      glm::vec3 GetSpecular() const;

      virtual ShaderData GetShaderData() const;

    protected:

      Light() = default;
      Light(glm::vec3 color, float ambient, float diffuse, float specular);
      Light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);

      ShaderData data;

  }; // class Light

  class DirectionalLight : public Light {

    public:

      virtual ShaderData GetShaderData() const;

      DirectionalLight();
      DirectionalLight(
          glm::vec3 color, float ambient, float diffuse, float specular);
      DirectionalLight(
          glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);

  }; // class DirectionalLight

  class PointLight : public Light {

    public:

      void SetConstant(float constant);
      void SetLinear(float linear);
      void SetQuadratic(float quadratic);

      float GetConstant() const;
      float GetLinear() const;
      float GetQuadratic() const;

      virtual ShaderData GetShaderData() const;

      PointLight();
      PointLight(
          glm::vec3 color, float ambient, float diffuse, float specular);
      PointLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);

  }; // class PointLight

  class SpotLight : public Light {

    public:

      void SetCutoff(float cutoff);
      void SetFeather(float feather);

      float GetCutoff() const;
      float GetFeather() const;

      virtual ShaderData GetShaderData() const;

      SpotLight();
      SpotLight(glm::vec3 color, float ambient, float diffuse, float specular);
      SpotLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);

    private:

      void SetDefaultProperties();

      float cutoff = glm::radians(30.f); // Radians of light cone.
      float feather = glm::radians(5.f); // Radians of cone edge fading.

  }; // class SpotLight

} // namespace dg

