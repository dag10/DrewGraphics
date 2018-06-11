//
//  materials/CubemapMirrorMaterial.cpp
//

#include "dg/materials/CubemapMirrorMaterial.h"
#include "dg/RasterizerState.h"

std::shared_ptr<dg::Shader> dg::CubemapMirrorMaterial::cubemapMirrorShader =
    nullptr;

dg::CubemapMirrorMaterial::CubemapMirrorMaterial() : Material() {
  if (cubemapMirrorShader == nullptr) {
#if defined(_OPENGL)
    cubemapMirrorShader = dg::Shader::FromFiles(
        "assets/shaders/cubemap-mirror.v.glsl",
        "assets/shaders/cubemap-mirror.f.glsl");
#elif defined(_DIRECTX)
    cubemapMirrorShader = dg::Shader::FromFiles("CubemapMirrorVertexShader.cso",
                                                "CubemapMirrorPixelShader.cso");
#endif
  }

  shader = CubemapMirrorMaterial::cubemapMirrorShader;

  SetUVScale   (glm::vec2(1));
  SetNormalMap (nullptr);
}

dg::CubemapMirrorMaterial::CubemapMirrorMaterial(
    std::shared_ptr<Cubemap> cubemap)
    : CubemapMirrorMaterial() {
  SetCubemap(cubemap);
}

dg::CubemapMirrorMaterial::CubemapMirrorMaterial(CubemapMirrorMaterial& other)
    : Material(other) {
}

dg::CubemapMirrorMaterial::CubemapMirrorMaterial(
    CubemapMirrorMaterial &&other) {
  *this = std::move(other);
}

dg::CubemapMirrorMaterial &dg::CubemapMirrorMaterial::operator=(
    CubemapMirrorMaterial &other) {
  *this = CubemapMirrorMaterial(other);
  return *this;
}

dg::CubemapMirrorMaterial &dg::CubemapMirrorMaterial::operator=(
    CubemapMirrorMaterial &&other) {
  swap(*this, other);
  return *this;
}

void dg::swap(CubemapMirrorMaterial& first, CubemapMirrorMaterial& second) {
  using std::swap;
  swap((Material&)first, (Material&)second);
}

void dg::CubemapMirrorMaterial::Use() const {
  Material::Use();
}

void dg::CubemapMirrorMaterial::SetUVScale(glm::vec2 scale) {
#if defined(_OPENGL)
  SetProperty("_UVScale", scale);
#elif defined(_DIRECTX)
  SetProperty("uvScale", scale);
#endif
}

void dg::CubemapMirrorMaterial::SetCubemap(std::shared_ptr<Cubemap> cubemap) {
  SetProperty("_Cubemap", cubemap, (int)TexUnitHints::CUBEMAP);
}

void dg::CubemapMirrorMaterial::SetNormalMap(
    std::shared_ptr<Texture> normalMap) {
#if defined(_OPENGL)
  if (normalMap == nullptr) {
    SetProperty("_Material.useNormalMap", false);
    ClearProperty("_Material.normalMap");
  } else {
    SetProperty("_Material.useNormalMap", true);
    SetProperty("_Material.normalMap", normalMap, (int)TexUnitHints::NORMAL);
  }
#elif defined(_DIRECTX)
  if (normalMap == nullptr) {
    SetProperty("useNormalMap", false);
    ClearProperty("normalTexture");
  } else {
    SetProperty("useNormalMap", true);
    SetProperty("normalTexture", normalMap);
  }
#endif
}
