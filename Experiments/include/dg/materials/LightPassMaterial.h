//
//  materials/LightPassMaterial.h
//

#pragma once

#include <memory>
#include "dg/Material.h"
#include "dg/Shader.h"
#include "dg/Texture.h"

namespace dg {

  class LightPassMaterial : public Material {

    public:

      LightPassMaterial();

      LightPassMaterial(LightPassMaterial& other);
      LightPassMaterial(LightPassMaterial&& other);
      LightPassMaterial& operator=(LightPassMaterial& other);
      LightPassMaterial& operator=(LightPassMaterial&& other);
      friend void swap(LightPassMaterial &first,
                       LightPassMaterial &second);  // nothrow

      void Use() const;

      void SetAlbedoTexture(std::shared_ptr<Texture> texture);
      void SetWorldPositionTexture(std::shared_ptr<Texture> texture);
      void SetNormalTexture(std::shared_ptr<Texture> texture);
      void SetSpecularTexture(std::shared_ptr<Texture> texture);
      void SetSSAOTexture(std::shared_ptr<Texture> texture);
      void SetDepthTexture(std::shared_ptr<Texture> texture);

      void SetEnableSSAO(bool enableSSAO);

    private:

      static std::shared_ptr<Shader> lightPassShader;

  }; // class LightPassMaterial

} // namespace dg
