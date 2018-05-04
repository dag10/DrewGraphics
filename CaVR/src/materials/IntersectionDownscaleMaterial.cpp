//
//  materials/IntersectionDownscaleMaterial.cpp
//

#include "cavr/materials/IntersectionDownscaleMaterial.h"

std::shared_ptr<dg::Shader>
    cavr::IntersectionDownscaleMaterial::intersectionDownscaleShader = nullptr;

cavr::IntersectionDownscaleMaterial::IntersectionDownscaleMaterial(
    std::shared_ptr<dg::Texture> texture)
    : Material() {
  if (intersectionDownscaleShader == nullptr) {
#if defined(_OPENGL)
    intersectionDownscaleShader =
        dg::Shader::FromFiles("assets/shaders/IntersectionDownscale/vert.glsl",
                              "assets/shaders/IntersectionDownscale/frag.glsl");
#elif defined(_DIRECTX)
    intersectionDownscaleShader =
        dg::Shader::FromFiles("IntersectionDownscaleVertexShader.cso",
                              "IntersectionDownscalePixelShader.cso");
#endif
  }

  SetTexture(texture);

  rasterizerOverride.SetCullMode(dg::RasterizerState::CullMode::OFF);

  shader = IntersectionDownscaleMaterial::intersectionDownscaleShader;
}

cavr::IntersectionDownscaleMaterial::IntersectionDownscaleMaterial(
    IntersectionDownscaleMaterial& other)
    : Material(other) {}

cavr::IntersectionDownscaleMaterial::IntersectionDownscaleMaterial(
    IntersectionDownscaleMaterial&& other) {
  *this = std::move(other);
}

cavr::IntersectionDownscaleMaterial& cavr::IntersectionDownscaleMaterial::
operator=(IntersectionDownscaleMaterial& other) {
  *this = IntersectionDownscaleMaterial(other);
  return *this;
}

cavr::IntersectionDownscaleMaterial& cavr::IntersectionDownscaleMaterial::
operator=(IntersectionDownscaleMaterial&& other) {
  swap(*this, other);
  return *this;
}

void cavr::swap(IntersectionDownscaleMaterial& first,
                IntersectionDownscaleMaterial& second) {
  using std::swap;
  swap((dg::Material&)first, (dg::Material&)second);
}

void cavr::IntersectionDownscaleMaterial::SetTexture(
    std::shared_ptr<dg::Texture> texture) {
#if defined(_OPENGL)
  SetProperty("_Texture", texture);
#elif defined(_DIRECTX)
  SetProperty("intersectionTexture", texture);
#endif
}
