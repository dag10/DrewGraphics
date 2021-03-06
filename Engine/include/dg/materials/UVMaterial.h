//
//  materials/UVMaterial.h
//
#pragma once

#include <memory>
#include "dg/Material.h"
#include "dg/Shader.h"
#include "dg/Texture.h"

namespace dg {

  class UVMaterial : public Material {

    public:

      static std::shared_ptr<Shader> GetStaticShader();

      UVMaterial();

      UVMaterial(UVMaterial& other);
      UVMaterial(UVMaterial&& other);
      UVMaterial& operator=(UVMaterial& other);
      UVMaterial& operator=(UVMaterial&& other);
      friend void swap(UVMaterial& first, UVMaterial& second); // nothrow

      void Use() const;

    private:

      static std::shared_ptr<Shader> uvShader;

  }; // class Material

} // namespace dg

