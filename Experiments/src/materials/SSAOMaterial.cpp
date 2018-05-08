//
//  materials/SSAOMaterial.cpp
//

#include "dg/materials/SSAOMaterial.h"
#include "dg/RasterizerState.h"

std::shared_ptr<dg::Shader> dg::SSAOMaterial::ssaoShader = nullptr;

dg::SSAOMaterial::SSAOMaterial() : Material() {
  if (ssaoShader == nullptr) {
#if defined(_OPENGL)
    ssaoShader = dg::Shader::FromFiles("assets/shaders/ssao.v.glsl",
                                       "assets/shaders/ssao.f.glsl");
#elif defined(_DIRECTX)
    ssaoShader =
        dg::Shader::FromFiles("SSAOVertexShader.cso", "SSAOPixelShader.cso");
#endif
  }

  shader = SSAOMaterial::ssaoShader;

  queue = RenderQueue::Overlay;
  rasterizerOverride.SetDepthFunc(RasterizerState::DepthFunc::ALWAYS);
}

dg::SSAOMaterial::SSAOMaterial(SSAOMaterial &other)
    : Material(other) {}

dg::SSAOMaterial::SSAOMaterial(SSAOMaterial &&other) {
  *this = std::move(other);
}

dg::SSAOMaterial &dg::SSAOMaterial::operator=(
    SSAOMaterial &other) {
  *this = SSAOMaterial(other);
  return *this;
}

dg::SSAOMaterial &dg::SSAOMaterial::operator=(
    SSAOMaterial &&other) {
  swap(*this, other);
  return *this;
}

void dg::swap(SSAOMaterial &first, SSAOMaterial &second) {
  using std::swap;
  swap((Material &)first, (Material &)second);
}

void dg::SSAOMaterial::Use() const { Material::Use(); }

void dg::SSAOMaterial::SetViewPositionTexture(std::shared_ptr<Texture> texture) {
#if defined(_OPENGL)
  SetProperty("_ViewPositionTexture", texture);
#elif defined(_DIRECTX)
  SetProperty("viewPositionTexture", texture);
#endif
}

void dg::SSAOMaterial::SetNormalTexture(std::shared_ptr<Texture> texture) {
#if defined(_OPENGL)
  SetProperty("_NormalTexture", texture);
#elif defined(_DIRECTX)
  SetProperty("normalTexture", texture);
#endif
}

void dg::SSAOMaterial::SetNoiseTexture(std::shared_ptr<Texture> texture) {
#if defined(_OPENGL)
  SetProperty("_NoiseTexture", texture);
#elif defined(_DIRECTX)
  SetProperty("noiseTexture", texture);
#endif
}
