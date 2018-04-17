//
//  materials/SphereIntersectionMaterial.cpp
//

#include "cavr/materials/SphereIntersectionMaterial.h"

std::shared_ptr<dg::Shader>
    cavr::SphereIntersectionMaterial::sphereIntersectionShader = nullptr;

cavr::SphereIntersectionMaterial::SphereIntersectionMaterial() : Material() {
  if (sphereIntersectionShader == nullptr) {
#if defined(_OPENGL)
    sphereIntersectionShader =
        dg::Shader::FromFiles("assets/shaders/SphereIntersection/vert.glsl",
                              "assets/shaders/SphereIntersection/frag.glsl");
#elif defined(_DIRECTX)
    sphereIntersectionShader =
        dg::Shader::FromFiles("SphereIntersectionVertexShader.cso",
                              "SphereIntersectionPixelShader.cso");
#endif
  }

  shader = SphereIntersectionMaterial::sphereIntersectionShader;
}

cavr::SphereIntersectionMaterial::SphereIntersectionMaterial(
    SphereIntersectionMaterial& other)
    : Material(other) {}

cavr::SphereIntersectionMaterial::SphereIntersectionMaterial(
    SphereIntersectionMaterial&& other) {
  *this = std::move(other);
}

cavr::SphereIntersectionMaterial& cavr::SphereIntersectionMaterial::operator=(
    SphereIntersectionMaterial& other) {
  *this = SphereIntersectionMaterial(other);
  return *this;
}

cavr::SphereIntersectionMaterial& cavr::SphereIntersectionMaterial::operator=(
    SphereIntersectionMaterial&& other) {
  swap(*this, other);
  return *this;
}

void cavr::swap(SphereIntersectionMaterial& first,
                SphereIntersectionMaterial& second) {
  using std::swap;
  swap((dg::Material&)first, (dg::Material&)second);
}
