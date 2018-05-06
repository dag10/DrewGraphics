//
//  materials/ScreenQuadMaterial.cpp
//

#include "dg/materials/ScreenQuadMaterial.h"
#include "dg/RasterizerState.h"

std::shared_ptr<dg::Shader> dg::ScreenQuadMaterial::screenQuadShader = nullptr;

dg::ScreenQuadMaterial::ScreenQuadMaterial() : Material() {
  if (screenQuadShader == nullptr) {
#if defined(_OPENGL)
    screenQuadShader = dg::Shader::FromFiles(
        "assets/shaders/screenquad.v.glsl", "assets/shaders/screenquad.f.glsl");
#elif defined(_DIRECTX)
    screenQuadShader = dg::Shader::FromFiles("ScreenQuadVertexShader.cso",
                                             "ScreenQuadPixelShader.cso");
#endif
  }

  shader = ScreenQuadMaterial::screenQuadShader;

  queue = RenderQueue::Overlay;
  rasterizerOverride.SetDepthFunc(RasterizerState::DepthFunc::ALWAYS);
}

dg::ScreenQuadMaterial::ScreenQuadMaterial(
  glm::vec3 color, glm::vec2 scale, glm::vec2 offset)
  : ScreenQuadMaterial() {
  SetColor(color);
  SetScale(scale);
  SetOffset(offset);
  SetRedChannelOnly(false);
}

dg::ScreenQuadMaterial::ScreenQuadMaterial(
  std::shared_ptr<Texture> texture, glm::vec2 scale, glm::vec2 offset)
  : ScreenQuadMaterial() {
  SetTexture(texture);
  SetScale(scale);
  SetOffset(offset);
  SetRedChannelOnly(false);
}

dg::ScreenQuadMaterial::ScreenQuadMaterial(ScreenQuadMaterial& other)
  : Material(other) {
}

dg::ScreenQuadMaterial::ScreenQuadMaterial(ScreenQuadMaterial&& other) {
  *this = std::move(other);
}

dg::ScreenQuadMaterial& dg::ScreenQuadMaterial::operator=(
    ScreenQuadMaterial& other) {
  *this = ScreenQuadMaterial(other);
  return *this;
}

dg::ScreenQuadMaterial& dg::ScreenQuadMaterial::operator=(
    ScreenQuadMaterial&& other) {
  swap(*this, other);
  return *this;
}

void dg::swap(ScreenQuadMaterial& first, ScreenQuadMaterial& second) {
  using std::swap;
  swap((Material&)first, (Material&)second);
}

void dg::ScreenQuadMaterial::Use() const {
  Material::Use();
}

void dg::ScreenQuadMaterial::SetColor(glm::vec3 color) {
  SetProperty("_Color", color);
  SetProperty("_UseTexture", false);
}

void dg::ScreenQuadMaterial::SetTexture(std::shared_ptr<Texture> texture) {
  SetProperty("_UseTexture", true);
#if defined(_OPENGL)
  SetProperty("_Texture", texture);
#elif defined(_DIRECTX)
  SetProperty("quadTexture", texture);
#endif
}

void dg::ScreenQuadMaterial::SetScale(glm::vec2 scale) {
  SetProperty("_Scale", scale);
}

void dg::ScreenQuadMaterial::SetOffset(glm::vec2 offset) {
  SetProperty("_Offset", offset);
}

void dg::ScreenQuadMaterial::SetRedChannelOnly(bool useRedChannelOnly) {
  SetProperty("_RedChannelOnly", useRedChannelOnly);
}
