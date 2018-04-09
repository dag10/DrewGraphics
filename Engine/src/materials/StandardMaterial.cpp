//
//  materials/StandardMaterial.cpp
//

#include "dg/materials/StandardMaterial.h"
#include "dg/RasterizerState.h"

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

dg::StandardMaterial dg::StandardMaterial::WithTransparentColor(
    glm::vec4 color) {
  StandardMaterial material;
  material.SetDiffuse(color);
  material.rasterizerOverride = RasterizerState::AlphaBlending();
  material.queue = RenderQueue::Transparent;
  return material;
}

dg::StandardMaterial dg::StandardMaterial::WithWireframeColor(glm::vec3 color) {
  StandardMaterial material;
  material.SetDiffuse(color);
  material.SetLit(false);
  material.rasterizerOverride = RasterizerState::Wireframe();
  material.queue = RenderQueue::Wireframe;
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
#if defined(_OPENGL)
    standardShader = dg::Shader::FromFiles(
        "assets/shaders/standard.v.glsl",
        "assets/shaders/standard.f.glsl");
#elif defined(_DIRECTX)
    standardShader = dg::Shader::FromFiles("StandardVertexShader.cso",
                                           "StandardPixelShader.cso");
#endif
  }

  shader = StandardMaterial::standardShader;

  SetUVScale   (glm::vec2(1));
  SetLit       (true);
  SetDiffuse   (1.0f);
  SetSpecular  (0.0f);
  SetShininess (32.0f);
  SetNormalMap (nullptr);
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
#if defined(_OPENGL)
  SetProperty("_UVScale", scale);
#elif defined(_DIRECTX)
  SetProperty("uvScale", scale);
#endif
}

void dg::StandardMaterial::SetLit(bool lit) {
#if defined(_OPENGL)
  SetProperty("_Material.lit", lit);
#elif defined(_DIRECTX)
  SetProperty("lit", lit);
#endif
}

void dg::StandardMaterial::SetDiffuse(float diffuse) {
  SetDiffuse(glm::vec3(diffuse));
}

void dg::StandardMaterial::SetDiffuse(glm::vec3 diffuse) {
  SetDiffuse(glm::vec4(diffuse, 1));
}

void dg::StandardMaterial::SetDiffuse(glm::vec4 diffuse) {
#if defined(_OPENGL)
  SetProperty("_Material.useDiffuseMap", false);
  SetProperty("_Material.diffuse", diffuse);
#elif defined(_DIRECTX)
  SetProperty("useDiffuseMap", false);
  SetProperty("diffuse", diffuse);
#endif
}

void dg::StandardMaterial::SetDiffuse(std::shared_ptr<Texture> diffuseMap) {
#if defined(_OPENGL)
  if (diffuseMap == nullptr) {
    SetProperty("_Material.useDiffuseMap", false);
    ClearProperty("_Material.diffuseMap");
  } else {
    SetProperty("_Material.useDiffuseMap", true);
    SetProperty("_Material.diffuseMap", diffuseMap, (int)TexUnitHints::DIFFUSE);
  }
#elif defined(_DIRECTX)
  if (diffuseMap == nullptr) {
    SetProperty("_Material.useDiffuseMap", false);
    ClearProperty("_Material.diffuseMap");
  } else {
    SetProperty("useDiffuseMap", true);
    SetProperty("diffuseTexture", diffuseMap);
  }
#endif
}

void dg::StandardMaterial::SetSpecular(float specular) {
  SetSpecular(glm::vec3(specular));
}

void dg::StandardMaterial::SetSpecular(glm::vec3 specular) {
#if defined(_OPENGL)
  SetProperty("_Material.useSpecularMap", false);
  SetProperty("_Material.specular", specular);
  ClearProperty("_Material.specularMap");
#elif defined(_DIRECTX)
  SetProperty("useSpecularMap", false);
  SetProperty("specular", specular);
  ClearProperty("specularTexture");
#endif
}

void dg::StandardMaterial::SetSpecular(std::shared_ptr<Texture> specularMap) {
#if defined(_OPENGL)
  if (specularMap == nullptr) {
    SetProperty("_Material.useSpecularMap", false);
    ClearProperty("_Material.specularMap");
  } else {
    SetProperty("_Material.useSpecularMap", true);
    SetProperty("_Material.specularMap", specularMap,
                (int)TexUnitHints::SPECULAR);
  }
#elif defined(_DIRECTX)
  if (specularMap == nullptr) {
    SetProperty("useSpecularMap", false);
    ClearProperty("specularTexture");
  } else {
    SetProperty("useSpecularMap", true);
    SetProperty("specularTexture", specularMap);
  }
#endif
}

void dg::StandardMaterial::SetNormalMap(std::shared_ptr<Texture> normalMap) {
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

void dg::StandardMaterial::SetShininess(float shininess) {
#if defined(_OPENGL)
  SetProperty("_Material.shininess", shininess);
#elif defined(_DIRECTX)
  SetProperty("shininess", shininess);
#endif
}
