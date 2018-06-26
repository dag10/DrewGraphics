//
//  materials/ShaderReplacedMaterial.h
//

#pragma once

#include <memory>
#include "dg/Material.h"
#include "dg/Shader.h"
#include "dg/Texture.h"

namespace dg {

  class ShaderReplacedMaterial : public Material {

    public:

      ShaderReplacedMaterial() = default;
      ShaderReplacedMaterial(std::shared_ptr<Material> material,
                             std::shared_ptr<Shader> newShader);

      ShaderReplacedMaterial(ShaderReplacedMaterial& other);
      ShaderReplacedMaterial(ShaderReplacedMaterial&& other);
      ShaderReplacedMaterial& operator=(ShaderReplacedMaterial& other);
      ShaderReplacedMaterial& operator=(ShaderReplacedMaterial&& other);
      friend void swap(ShaderReplacedMaterial &first,
                       ShaderReplacedMaterial &second);  // nothrow

    protected:

      virtual void SendShaderProperties() const;

    private:

      std::shared_ptr<Material> material = nullptr;

  }; // class Material

} // namespace dg
