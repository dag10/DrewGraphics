//
//  StandardMaterial.cpp
//

#include "StandardMaterial.h"

std::shared_ptr<dg::Shader> dg::StandardMaterial::standardShader = nullptr;

dg::StandardMaterial dg::StandardMaterial::WithColor(glm::vec3 color) {
  StandardMaterial material;
  material.SetAlbedo(color);
  return material;
}

dg::StandardMaterial dg::StandardMaterial::WithColor(glm::vec4 color) {
  StandardMaterial material;
  material.SetAlbedo(color);
  return material;
}

dg::StandardMaterial dg::StandardMaterial::WithTexture(
    std::shared_ptr<Texture> texture) {
  StandardMaterial material;
  material.SetAlbedo(texture);
  return material;
}

dg::StandardMaterial::StandardMaterial() : Material() {
  if (standardShader == nullptr) {
    standardShader = std::make_shared<Shader>(dg::Shader::FromFiles(
          "assets/shaders/standard.v.glsl",
          "assets/shaders/standard.f.glsl"));
  }

  shader = StandardMaterial::standardShader;

  SetUVScale(glm::vec2(1));

  SetLit(true);
  SetAlbedo(glm::vec4(1));
  SetAmbient (0.8f);
  SetDiffuse (1.0f);
  SetSpecular(0.0f);
}

dg::StandardMaterial::StandardMaterial(StandardMaterial& other)
    : Material(other) {
}

dg::StandardMaterial::StandardMaterial(StandardMaterial&& other) {
  *this = std::move(other);
}

dg::StandardMaterial& dg::StandardMaterial::operator=(StandardMaterial& other) {
  *this = StandardMaterial(other);
  return *this;
}

dg::StandardMaterial& dg::StandardMaterial::operator=(StandardMaterial&& other) {
  swap(*this, other);
  return *this;
}

void dg::swap(StandardMaterial& first, StandardMaterial& second) {
  using std::swap;
  swap((Material&)first, (Material&)second);
}

void dg::StandardMaterial::Use() const {
  Material::Use();
}

void dg::StandardMaterial::SetUVScale(glm::vec2 scale) {
  SetProperty("UVScale", scale);
}

void dg::StandardMaterial::SetLit(bool lit) {
  SetProperty("Lit", lit);
}

void dg::StandardMaterial::SetAlbedo(std::shared_ptr<Texture> value) {
  SetProperty("AlbedoSampler", true);
  SetProperty("MainTex", value);
}

void dg::StandardMaterial::SetAlbedo(glm::vec3 value) {
  SetAlbedo(glm::vec4(value, 1));
}

void dg::StandardMaterial::SetAlbedo(glm::vec4 value) {
  SetProperty("AlbedoSampler", false);
  SetProperty("Albedo", value);
}

void dg::StandardMaterial::SetAmbient(float ambient) {
  SetProperty("AmbientStrength", ambient);
}

void dg::StandardMaterial::SetDiffuse(float diffuse) {
  SetProperty("DiffuseStrength", diffuse);
}

void dg::StandardMaterial::SetSpecular(float specular) {
  SetProperty("SpecularStrength", specular);
}

