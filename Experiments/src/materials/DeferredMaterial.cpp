//
//  materials/DeferredMaterial.cpp
//

#include "dg/materials/DeferredMaterial.h"
#include "dg/RasterizerState.h"

std::shared_ptr<dg::Shader> dg::DeferredMaterial::deferredShader = nullptr;

dg::DeferredMaterial dg::DeferredMaterial::WithColor(glm::vec3 color) {
  DeferredMaterial material;
  material.SetDiffuse(color);
  return material;
}

dg::DeferredMaterial dg::DeferredMaterial::WithColor(glm::vec4 color) {
  DeferredMaterial material;
  material.SetDiffuse(color);
  return material;
}

dg::DeferredMaterial dg::DeferredMaterial::WithTransparentColor(
    glm::vec4 color) {
  DeferredMaterial material;
  material.SetDiffuse(color);
  material.rasterizerOverride += RasterizerState::AlphaBlending();
  material.queue = RenderQueue::Transparent;
  return material;
}

dg::DeferredMaterial dg::DeferredMaterial::WithWireframeColor(glm::vec3 color) {
  DeferredMaterial material = WithColor(color);
  material.SetLit(false);
  material.rasterizerOverride += RasterizerState::Wireframe();
  material.queue = RenderQueue::Wireframe;
  return material;
}

dg::DeferredMaterial dg::DeferredMaterial::WithWireframeColor(glm::vec4 color) {
  DeferredMaterial material = WithTransparentColor(color);
  material.SetLit(false);
  material.rasterizerOverride += RasterizerState::Wireframe();
  material.queue = RenderQueue::Wireframe;
  return material;
}

dg::DeferredMaterial dg::DeferredMaterial::WithTexture(
    std::shared_ptr<Texture> texture) {
  DeferredMaterial material;
  material.SetDiffuse(texture);
  return material;
}

dg::DeferredMaterial::DeferredMaterial() : Material() {
  if (deferredShader == nullptr) {
#if defined(_OPENGL)
    deferredShader = dg::Shader::FromFiles(
        "assets/shaders/deferred.v.glsl",
        "assets/shaders/deferred.f.glsl");
#elif defined(_DIRECTX)
    deferredShader = dg::Shader::FromFiles("DeferredVertexShader.cso",
                                           "DeferredPixelShader.cso");
#endif
  }

  shader = DeferredMaterial::deferredShader;

  SetUVScale   (glm::vec2(1));
  SetLit       (true);
  SetDiffuse   (1.0f);
  SetSpecular  (0.0f);
  SetShininess (32.0f);
  SetNormalMap (nullptr);
}

dg::DeferredMaterial::DeferredMaterial(DeferredMaterial& other)
    : Material(other) {
}

dg::DeferredMaterial::DeferredMaterial(DeferredMaterial&& other) {
  *this = std::move(other);
}

dg::DeferredMaterial& dg::DeferredMaterial::operator=(DeferredMaterial& other) {
  *this = DeferredMaterial(other);
  return *this;
}

dg::DeferredMaterial& dg::DeferredMaterial::operator=(DeferredMaterial&& other) {
  swap(*this, other);
  return *this;
}

void dg::swap(DeferredMaterial& first, DeferredMaterial& second) {
  using std::swap;
  swap((Material&)first, (Material&)second);
}

void dg::DeferredMaterial::Use() const {
  Material::Use();
}

void dg::DeferredMaterial::SetUVScale(glm::vec2 scale) {
#if defined(_OPENGL)
  SetProperty("_UVScale", scale);
#elif defined(_DIRECTX)
  SetProperty("uvScale", scale);
#endif
}

void dg::DeferredMaterial::SetLit(bool lit) {
#if defined(_OPENGL)
  SetProperty("_Material.lit", lit);
#elif defined(_DIRECTX)
  SetProperty("lit", lit);
#endif
}

void dg::DeferredMaterial::SetDiffuse(float diffuse) {
  SetDiffuse(glm::vec3(diffuse));
}

void dg::DeferredMaterial::SetDiffuse(glm::vec3 diffuse) {
  SetDiffuse(glm::vec4(diffuse, 1));
}

void dg::DeferredMaterial::SetDiffuse(glm::vec4 diffuse) {
#if defined(_OPENGL)
  SetProperty("_Material.useDiffuseMap", false);
  SetProperty("_Material.diffuse", diffuse);
#elif defined(_DIRECTX)
  SetProperty("useDiffuseMap", false);
  SetProperty("diffuse", diffuse);
#endif
}

void dg::DeferredMaterial::SetDiffuse(std::shared_ptr<Texture> diffuseMap) {
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

void dg::DeferredMaterial::SetSpecular(float specular) {
  SetSpecular(glm::vec3(specular));
}

void dg::DeferredMaterial::SetSpecular(glm::vec3 specular) {
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

void dg::DeferredMaterial::SetSpecular(std::shared_ptr<Texture> specularMap) {
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

void dg::DeferredMaterial::SetNormalMap(std::shared_ptr<Texture> normalMap) {
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

void dg::DeferredMaterial::SetShininess(float shininess) {
#if defined(_OPENGL)
  SetProperty("_Material.shininess", shininess);
#elif defined(_DIRECTX)
  SetProperty("shininess", shininess);
#endif
}
