//
//  raytracing/TraceableUVMaterial.h
//
#pragma once

#include <Shader.h>
#include <Texture.h>
#include <materials/UVMaterial.h>
#include <raytracing/TraceableMaterial.h>
#include <memory>

namespace dg {

class TraceableUVMaterial : public UVMaterial, public TraceableMaterial {

  public:

    TraceableUVMaterial();

    TraceableUVMaterial(TraceableUVMaterial &other);
    TraceableUVMaterial(TraceableUVMaterial &&other);
    TraceableUVMaterial &operator=(TraceableUVMaterial &other);
    TraceableUVMaterial &operator=(TraceableUVMaterial &&other);
    friend void swap(TraceableUVMaterial &first,
                     TraceableUVMaterial &second);  // nothrow

    virtual glm::vec3 Shade(const RayResult& rayres) const;

  }; // class Material

} // namespace dg
