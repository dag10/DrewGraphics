//
//  materials/StandardMaterial.cpp
//

#include <materials/StandardMaterial.h>

std::shared_ptr<dg::Shader> dg::StandardMaterial::standardShader = nullptr;

dg::StandardMaterial dg::StandardMaterial::WithColor(glm::vec3 color) {
  StandardMaterial material;
  material.SetDiffuse(color);
  return material;
}

dg::StandardMaterial dg::StandardMaterial::WithColor(glm::vec4 color) {
  StandardMaterial material;
  material.SetDiffuse(color);
  return material;
}

dg::StandardMaterial dg::StandardMaterial::WithTexture(
    std::shared_ptr<Texture> texture) {
  StandardMaterial material;
  material.SetDiffuse(texture);
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
  SetDiffuse   (1.0f);
  SetSpecular  (0.0f);
  SetShininess (32.0f);
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
  SetProperty("_UVScale", scale);
}

void dg::StandardMaterial::SetLit(bool lit) {
  SetProperty("_Material.lit", lit);
}

void dg::StandardMaterial::SetDiffuse(float diffuse) {
  SetDiffuse(glm::vec3(diffuse));
}

void dg::StandardMaterial::SetDiffuse(glm::vec3 diffuse) {
  SetProperty("_Material.useDiffuseMap", false);
  SetProperty("_Material.diffuse", diffuse);
}

void dg::StandardMaterial::SetDiffuse(std::shared_ptr<Texture> diffuseMap) {
  if (diffuseMap == nullptr) {
    SetProperty("_Material.useDiffuseMap", false);
  } else {
    SetProperty("_Material.useDiffuseMap", true);
    SetProperty("_Material.diffuseMap", diffuseMap);
  }
}

void dg::StandardMaterial::SetSpecular(float specular) {
  SetSpecular(glm::vec3(specular));
}

void dg::StandardMaterial::SetSpecular(glm::vec3 specular) {
  SetProperty("_Material.useSpecularMap", false);
  SetProperty("_Material.specular", specular);
}

void dg::StandardMaterial::SetSpecular(std::shared_ptr<Texture> specularMap) {
  if (specularMap == nullptr) {
    SetProperty("_Material.useSpecularMap", false);
  } else {
    SetProperty("_Material.useSpecularMap", true);
    SetProperty("_Material.specularMap", specularMap);
  }
}

void dg::StandardMaterial::SetNormalMap(std::shared_ptr<Texture> normalMap) {
  if (normalMap == nullptr) {
    SetProperty("_Material.useNormalMap", false);
  } else {
    SetProperty("_Material.useNormalMap", true);
    SetProperty("_Material.normalMap", normalMap);
  }
}

void dg::StandardMaterial::SetShininess(float shininess) {
  SetProperty("_Material.shininess", shininess);
}

