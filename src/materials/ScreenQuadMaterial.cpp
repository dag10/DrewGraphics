//
//  materials/ScreenQuadMaterial.cpp
//

#include <materials/ScreenQuadMaterial.h>

std::shared_ptr<dg::Shader> dg::ScreenQuadMaterial::uvShader = nullptr;

dg::ScreenQuadMaterial::ScreenQuadMaterial() : Material() {
  if (uvShader == nullptr) {
    uvShader = std::make_shared<Shader>(dg::Shader::FromFiles(
          "assets/shaders/screenquad.v.glsl",
          "assets/shaders/screenquad.f.glsl"));
  }

  shader = ScreenQuadMaterial::uvShader;
}

dg::ScreenQuadMaterial::ScreenQuadMaterial(std::shared_ptr<Texture> texture)
  : ScreenQuadMaterial() {
  SetTexture(texture);
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

void dg::ScreenQuadMaterial::SetTexture(std::shared_ptr<Texture> texture) {
  SetProperty("_Texture", texture);
}

