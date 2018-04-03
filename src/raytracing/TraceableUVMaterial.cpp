//
//  raytracing/TraceableUVMaterial.cpp
//

#include <raytracing/TraceableUVMaterial.h>
#include <raytracing/Rays.h>

dg::TraceableUVMaterial::TraceableUVMaterial()
    : UVMaterial(), TraceableMaterial() {}

dg::TraceableUVMaterial::TraceableUVMaterial(
    TraceableUVMaterial &other)
    : UVMaterial(other) {}

dg::TraceableUVMaterial::TraceableUVMaterial(
    TraceableUVMaterial &&other) {
  *this = std::move(other);
}

dg::TraceableUVMaterial &dg::TraceableUVMaterial::operator=(
    TraceableUVMaterial &other) {
  *this = TraceableUVMaterial(other);
  return *this;
}

dg::TraceableUVMaterial &dg::TraceableUVMaterial::operator=(
    TraceableUVMaterial &&other) {
  swap(*this, other);
  return *this;
}

void dg::swap(TraceableUVMaterial &first,
              TraceableUVMaterial &second) {
  using std::swap;
  swap((Material&)first, (Material&)second);
}

glm::vec3 dg::TraceableUVMaterial::Shade(const RayResult& rayres) const {
  return glm::vec3(rayres.interpolatedVertex.texCoord.x,
                   rayres.interpolatedVertex.texCoord.y, 0);
}
