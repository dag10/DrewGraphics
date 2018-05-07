//
//  materials/DeferredMaterial.h
//
#pragma once

#include <memory>
#include "dg/Material.h"
#include "dg/Shader.h"
#include "dg/Texture.h"

namespace dg {

  class DeferredMaterial : public Material {

    public:

      static DeferredMaterial WithColor(glm::vec3 color);
      static DeferredMaterial WithColor(glm::vec4 color);
      static DeferredMaterial WithTransparentColor(glm::vec4 color);
      static DeferredMaterial WithTexture(std::shared_ptr<Texture> texture);
      static DeferredMaterial WithWireframeColor(glm::vec3 color);
      static DeferredMaterial WithWireframeColor(glm::vec4 color);

      DeferredMaterial();

      DeferredMaterial(DeferredMaterial& other);
      DeferredMaterial(DeferredMaterial&& other);
      DeferredMaterial& operator=(DeferredMaterial& other);
      DeferredMaterial& operator=(DeferredMaterial&& other);
      friend void swap(DeferredMaterial &first,
                       DeferredMaterial &second);  // nothrow

      void Use() const;

      void SetUVScale(glm::vec2 scale);
      void SetLit(bool lit);
      void SetDiffuse(float diffuse);
      void SetDiffuse(glm::vec3 diffuse);
      void SetDiffuse(glm::vec4 diffuse);
      void SetDiffuse(std::shared_ptr<Texture> diffuseMap);
      void SetSpecular(float specular);
      void SetSpecular(glm::vec3 specular);
      void SetSpecular(std::shared_ptr<Texture> specularMap);
      void SetNormalMap(std::shared_ptr<Texture> normalMap);
      void SetShininess(float shininess);

    private:

      enum class TexUnitHints {
        DIFFUSE = (int)Material::TexUnitHints::END,
        SPECULAR,
        NORMAL,

        END,
      };

      static std::shared_ptr<Shader> deferredShader;

  }; // class Material

} // namespace dg
