//
//  materials/SSAOMaterial.h
//

#pragma once

#include <memory>
#include "dg/Material.h"
#include "dg/Shader.h"
#include "dg/Texture.h"

namespace dg {

  class SSAOMaterial : public Material {

    public:

      SSAOMaterial();

      SSAOMaterial(SSAOMaterial& other);
      SSAOMaterial(SSAOMaterial&& other);
      SSAOMaterial& operator=(SSAOMaterial& other);
      SSAOMaterial& operator=(SSAOMaterial&& other);
      friend void swap(SSAOMaterial &first,
                       SSAOMaterial &second);  // nothrow

      void Use() const;

      void SetPositionTexture(std::shared_ptr<Texture> texture);
      void SetNormalTexture(std::shared_ptr<Texture> texture);
      void SetNoiseTexture(std::shared_ptr<Texture> texture);

    private:

      static std::shared_ptr<Shader> ssaoShader;

  }; // class SSAOMaterial

} // namespace dg
