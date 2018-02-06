//
//  materials/ScreenQuadMaterial.h
//
#pragma once

#include <memory>
#include <Material.h>
#include <Shader.h>
#include <Texture.h>

namespace dg {

  class ScreenQuadMaterial : public Material {

    public:

      ScreenQuadMaterial();
      ScreenQuadMaterial(std::shared_ptr<Texture> texture);

      ScreenQuadMaterial(ScreenQuadMaterial& other);
      ScreenQuadMaterial(ScreenQuadMaterial&& other);
      ScreenQuadMaterial& operator=(ScreenQuadMaterial& other);
      ScreenQuadMaterial& operator=(ScreenQuadMaterial&& other);
      friend void swap(ScreenQuadMaterial& first, ScreenQuadMaterial& second); // nothrow

      void Use() const;

      void SetTexture(std::shared_ptr<Texture> texture);

    private:

      static std::shared_ptr<Shader> uvShader;

  }; // class Material

} // namespace dg

