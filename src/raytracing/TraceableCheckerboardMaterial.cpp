//
//  raytracing/TraceableCheckerboardMaterial.cpp
//

#include <raytracing/TraceableCheckerboardMaterial.h>
#include <raytracing/Rays.h>

dg::TraceableCheckerboardMaterial::TraceableCheckerboardMaterial()
    : CheckerboardMaterial() {}

dg::TraceableCheckerboardMaterial::TraceableCheckerboardMaterial(
    TraceableCheckerboardMaterial &other)
    : CheckerboardMaterial(other) {}

dg::TraceableCheckerboardMaterial::TraceableCheckerboardMaterial(
    TraceableCheckerboardMaterial &&other) {
  *this = std::move(other);
}

dg::TraceableCheckerboardMaterial &dg::TraceableCheckerboardMaterial::operator=(
    TraceableCheckerboardMaterial &other) {
  *this = TraceableCheckerboardMaterial(other);
  return *this;
}

dg::TraceableCheckerboardMaterial &dg::TraceableCheckerboardMaterial::operator=(
    TraceableCheckerboardMaterial &&other) {
  swap(*this, other);
  return *this;
}

void dg::swap(TraceableCheckerboardMaterial &first,
              TraceableCheckerboardMaterial &second) {
  using std::swap;
  swap((Material&)first, (Material&)second);
}

glm::vec3 dg::TraceableCheckerboardMaterial::Shade(
    const RayResult &rayres) const {
  glm::vec2 _Size = GetRequiredProperty("_Size")->value._vec2;
  glm::vec3 _Colors[2];
  _Colors[0] = GetRequiredProperty("_Colors[0]")->value._vec3;
  _Colors[1] = GetRequiredProperty("_Colors[1]")->value._vec3;

  glm::vec2 coord = rayres.interpolatedVertex.texCoord * _Size;
  int parity = int(coord.x) % 2 ^ int(coord.y) % 2;
  return _Colors[parity];
}
