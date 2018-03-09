//
//  raytracing/TraceableStandardMaterial.cpp
//

#include <raytracing/TraceableStandardMaterial.h>

dg::TraceableStandardMaterial dg::TraceableStandardMaterial::WithColor(
    glm::vec3 color) {
  TraceableStandardMaterial material;
  material.SetDiffuse(color);
  return material;
}

dg::TraceableStandardMaterial dg::TraceableStandardMaterial::WithColor(
    glm::vec4 color) {
  TraceableStandardMaterial material;
  material.SetDiffuse({ color.x, color.y, color.z });
  return material;
}

dg::TraceableStandardMaterial dg::TraceableStandardMaterial::WithTexture(
    std::shared_ptr<Texture> texture) {
  TraceableStandardMaterial material;
  material.SetDiffuse(texture);
  return material;
}

dg::TraceableStandardMaterial::TraceableStandardMaterial()
    : StandardMaterial() {}

dg::TraceableStandardMaterial::TraceableStandardMaterial(
    TraceableStandardMaterial &other)
    : StandardMaterial(other) {}

dg::TraceableStandardMaterial::TraceableStandardMaterial(TraceableStandardMaterial&& other) {
  *this = std::move(other);
}

dg::TraceableStandardMaterial& dg::TraceableStandardMaterial::operator=(TraceableStandardMaterial& other) {
  *this = TraceableStandardMaterial(other);
  return *this;
}

dg::TraceableStandardMaterial& dg::TraceableStandardMaterial::operator=(TraceableStandardMaterial&& other) {
  swap(*this, other);
  return *this;
}

void dg::swap(TraceableStandardMaterial& first, TraceableStandardMaterial& second) {
  using std::swap;
  swap((Material&)first, (Material&)second);
}

glm::vec3 dg::TraceableStandardMaterial::Shade(const RayResult& rayres) const {
  // TODO
  return glm::vec3(1, 0, 1);
}
