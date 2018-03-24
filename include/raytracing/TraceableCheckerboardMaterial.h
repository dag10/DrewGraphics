//
//  raytracing/TraceableCheckerboardMaterial.h
//

#pragma once

#include <Shader.h>
#include <Texture.h>
#include <materials/CheckerboardMaterial.h>
#include <raytracing/TraceableMaterial.h>
#include <memory>

namespace dg {

class TraceableCheckerboardMaterial : public CheckerboardMaterial,
                                  public TraceableMaterial {
  public:

    TraceableCheckerboardMaterial();

    TraceableCheckerboardMaterial(TraceableCheckerboardMaterial &other);
    TraceableCheckerboardMaterial(TraceableCheckerboardMaterial &&other);
    TraceableCheckerboardMaterial &operator=(
        TraceableCheckerboardMaterial &other);
    TraceableCheckerboardMaterial &operator=(
        TraceableCheckerboardMaterial &&other);
    friend void swap(TraceableCheckerboardMaterial &first,
                     TraceableCheckerboardMaterial &second); // nothrow

    virtual glm::vec3 Shade(const RayResult& rayres) const;

  }; // class TraceableCheckerboardMaterial

} // namespace dg
