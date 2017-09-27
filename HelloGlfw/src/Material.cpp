//
//  Material.cpp
//

#include "Material.h"

dg::Material::Material(Material& other) {
  this->shader = other.shader;

  this->texture = other.texture;
  this->uvScale = other.uvScale;
  this->invPortal = other.invPortal;

  this->lit = other.lit;
  this->albedo = other.albedo;
  this->lightColor = other.lightColor;
  this->ambientStrength = other.ambientStrength;
  this->diffuseStrength = other.diffuseStrength;
  this->specularStrength = other.specularStrength;
}

dg::Material::Material(Material&& other) {
  *this = std::move(other);
}

dg::Material& dg::Material::operator=(Material& other) {
  *this = Material(other);
  return *this;
}

dg::Material& dg::Material::operator=(Material&& other) {
  swap(*this, other);
  return *this;
}

void dg::swap(Material& first, Material& second) {
  using std::swap;
  swap(first.shader, second.shader);

  swap(first.texture, second.texture);
  swap(first.uvScale, second.uvScale);
  swap(first.invPortal, second.invPortal);

  swap(first.lit, second.lit);
  swap(first.albedo, second.albedo);
  swap(first.lightColor, second.lightColor);
  swap(first.ambientStrength, second.ambientStrength);
  swap(first.diffuseStrength, second.diffuseStrength);
  swap(first.specularStrength, second.specularStrength);
}

void dg::Material::Use() const {
  shader->Use();
  shader->SetVec2("UVScale", uvScale);
  shader->SetMat4("InvPortal", invPortal);
  shader->SetBool("Lit", lit);
  shader->SetVec3("Albedo", albedo);
  shader->SetVec3("LightColor", lightColor);
  shader->SetFloat("AmbientStrength", ambientStrength);
  shader->SetFloat("DiffuseStrength", diffuseStrength);
  shader->SetFloat("SpecularStrength", specularStrength);
  if (texture != nullptr) {
    shader->SetTexture(0, "MainTex", *texture);
  }
}

