//
//  materials/SphereIntersectionMaterial.h
//

#pragma once

#include <memory>
#include "dg/Material.h"
#include "dg/Shader.h"

namespace cavr {

  class SphereIntersectionMaterial : public dg::Material {

    public:

      SphereIntersectionMaterial();

      SphereIntersectionMaterial(SphereIntersectionMaterial& other);
      SphereIntersectionMaterial(SphereIntersectionMaterial&& other);
      SphereIntersectionMaterial& operator=(SphereIntersectionMaterial& other);
      SphereIntersectionMaterial& operator=(SphereIntersectionMaterial&& other);
      friend void swap(SphereIntersectionMaterial& first,
                       SphereIntersectionMaterial& second); // nothrow

    private:

      static std::shared_ptr<dg::Shader> sphereIntersectionShader;

  }; // class Material

} // namespace cavr

