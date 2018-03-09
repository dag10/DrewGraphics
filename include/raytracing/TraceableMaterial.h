//
//  raytracing/TraceableMaterial.h
//

#pragma once

#include <Lights.h>
#include <glm/glm.hpp>

namespace dg {

  class RayResult;

  class TraceableMaterial {

    public:

      virtual glm::vec3 Shade(const RayResult& rayres) const = 0;

  }; // class TraceableMaterial

} // namespace dg
