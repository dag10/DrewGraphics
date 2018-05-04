//
//  materials/IntersectionDownscaleMaterial.h
//

#pragma once

#include <memory>
#include "dg/Material.h"
#include "dg/Shader.h"
#include "dg/Texture.h"

namespace cavr {

  class IntersectionDownscaleMaterial : public dg::Material {

    public:

      IntersectionDownscaleMaterial(std::shared_ptr<dg::Texture> texture);

      IntersectionDownscaleMaterial(IntersectionDownscaleMaterial& other);
      IntersectionDownscaleMaterial(IntersectionDownscaleMaterial&& other);
      IntersectionDownscaleMaterial& operator=(IntersectionDownscaleMaterial& other);
      IntersectionDownscaleMaterial& operator=(IntersectionDownscaleMaterial&& other);
      friend void swap(IntersectionDownscaleMaterial& first,
                       IntersectionDownscaleMaterial& second); // nothrow

      void SetTexture(std::shared_ptr<dg::Texture> texture);

    private:

      static std::shared_ptr<dg::Shader> intersectionDownscaleShader;

  }; // class Material

} // namespace cavr