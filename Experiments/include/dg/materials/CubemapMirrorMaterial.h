/*
//
//  materials/CubemapMirrorMaterial.h
//
#pragma once

#include <memory>
#include "dg/Material.h"
#include "dg/Shader.h"
#include "dg/Texture.h"

namespace dg {

  class CubemapMirrorMaterial : public Material {

    public:

      CubemapMirrorMaterial();
      CubemapMirrorMaterial(std::shared_ptr<Cubemap> cubemap);

      CubemapMirrorMaterial(CubemapMirrorMaterial& other);
      CubemapMirrorMaterial(CubemapMirrorMaterial&& other);
      CubemapMirrorMaterial& operator=(CubemapMirrorMaterial& other);
      CubemapMirrorMaterial& operator=(CubemapMirrorMaterial&& other);
      friend void swap(CubemapMirrorMaterial &first,
                       CubemapMirrorMaterial &second);  // nothrow

      void Use() const;

      void SetUVScale(glm::vec2 scale);
      void SetCubemap(std::shared_ptr<Cubemap> cubemap);
      void SetNormalMap(std::shared_ptr<Texture> normalMap);

    private:

      enum class TexUnitHints {
        CUBEMAP = (int)Material::TexUnitHints::END,
        NORMAL,

        END,
      };

      static std::shared_ptr<Shader> cubemapMirrorShader;

  }; // class CubemapMirrorMaterial

} // namespace dg
*/
