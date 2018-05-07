//
//  materials/LightPassMaterial.cpp
//

#include "dg/materials/LightPassMaterial.h"
#include "dg/RasterizerState.h"

std::shared_ptr<dg::Shader> dg::LightPassMaterial::lightPassShader = nullptr;

dg::LightPassMaterial::LightPassMaterial() : Material() {
  if (lightPassShader == nullptr) {
#if defined(_OPENGL)
    lightPassShader = dg::Shader::FromFiles("assets/shaders/lightpass.v.glsl",
                                            "assets/shaders/lightpass.f.glsl");
#elif defined(_DIRECTX)
    lightPassShader = dg::Shader::FromFiles("LightPassVertexShader.cso",
                                            "LightPassPixelShader.cso");
#endif
  }

  shader = LightPassMaterial::lightPassShader;

  queue = RenderQueue::Overlay;
  rasterizerOverride.SetDepthFunc(RasterizerState::DepthFunc::ALWAYS);
}

dg::LightPassMaterial::LightPassMaterial(LightPassMaterial &other)
    : Material(other) {}

dg::LightPassMaterial::LightPassMaterial(LightPassMaterial &&other) {
  *this = std::move(other);
}

dg::LightPassMaterial &dg::LightPassMaterial::operator=(
    LightPassMaterial &other) {
  *this = LightPassMaterial(other);
  return *this;
}

dg::LightPassMaterial &dg::LightPassMaterial::operator=(
    LightPassMaterial &&other) {
  swap(*this, other);
  return *this;
}

void dg::swap(LightPassMaterial &first, LightPassMaterial &second) {
  using std::swap;
  swap((Material &)first, (Material &)second);
}

void dg::LightPassMaterial::Use() const { Material::Use(); }

void dg::LightPassMaterial::SetAlbedoTexture(std::shared_ptr<Texture> texture) {
#if defined(_OPENGL)
  SetProperty("_AlbedoTexture", texture);
#elif defined(_DIRECTX)
  SetProperty("albedoTexture", texture);
#endif
}

void dg::LightPassMaterial::SetPositionTexture(
    std::shared_ptr<Texture> texture) {
#if defined(_OPENGL)
  SetProperty("_PositionTexture", texture);
#elif defined(_DIRECTX)
  SetProperty("positionTexture", texture);
#endif
}

void dg::LightPassMaterial::SetNormalTexture(std::shared_ptr<Texture> texture) {
#if defined(_OPENGL)
  SetProperty("_NormalTexture", texture);
#elif defined(_DIRECTX)
  SetProperty("normalTexture", texture);
#endif
}

void dg::LightPassMaterial::SetSpecularTexture(
    std::shared_ptr<Texture> texture) {
#if defined(_OPENGL)
  SetProperty("_SpecularTexture", texture);
#elif defined(_DIRECTX)
  SetProperty("specularTexture", texture);
#endif
}

void dg::LightPassMaterial::SetDepthTexture(std::shared_ptr<Texture> texture) {
#if defined(_OPENGL)
  SetProperty("_DepthTexture", texture);
#elif defined(_DIRECTX)
  SetProperty("depthTexture", texture);
#endif
}
