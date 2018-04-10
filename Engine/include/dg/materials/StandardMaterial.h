//
//  materials/StandardMaterial.h
//
#pragma once

#include <memory>
#include "dg/Material.h"
#include "dg/Shader.h"
#include "dg/Texture.h"

namespace dg {

  class StandardMaterial : public Material {

    public:

      static StandardMaterial WithColor(glm::vec3 color);
      static StandardMaterial WithColor(glm::vec4 color);
      static StandardMaterial WithTransparentColor(glm::vec4 color);
      static StandardMaterial WithTexture(std::shared_ptr<Texture> texture);
      static StandardMaterial WithWireframeColor(glm::vec3 color);
      static StandardMaterial WithWireframeColor(glm::vec4 color);

      StandardMaterial();

      StandardMaterial(StandardMaterial& other);
      StandardMaterial(StandardMaterial&& other);
      StandardMaterial& operator=(StandardMaterial& other);
      StandardMaterial& operator=(StandardMaterial&& other);
      friend void swap(StandardMaterial &first,
                       StandardMaterial &second);  // nothrow

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

      static std::shared_ptr<Shader> standardShader;

  }; // class Material

} // namespace dg
