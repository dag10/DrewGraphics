//
//  materials/UVMaterial.cpp
//

#include "dg/materials/UVMaterial.h"

std::shared_ptr<dg::Shader> dg::UVMaterial::uvShader = nullptr;

dg::UVMaterial::UVMaterial() : Material() {
  if (uvShader == nullptr) {
#if defined(_OPENGL)
    uvShader = dg::Shader::FromFiles("assets/shaders/uv.v.glsl",
                                     "assets/shaders/uv.f.glsl");
#elif defined(_DIRECTX)
    uvShader = dg::Shader::FromFiles("UVVertexShader.cso", "UVPixelShader.cso");
#endif
  }

  shader = UVMaterial::uvShader;
}

dg::UVMaterial::UVMaterial(UVMaterial& other) : Material(other) {
}

dg::UVMaterial::UVMaterial(UVMaterial&& other) {
  *this = std::move(other);
}

dg::UVMaterial& dg::UVMaterial::operator=(UVMaterial& other) {
  *this = UVMaterial(other);
  return *this;
}

dg::UVMaterial& dg::UVMaterial::operator=(UVMaterial&& other) {
  swap(*this, other);
  return *this;
}

void dg::swap(UVMaterial& first, UVMaterial& second) {
  using std::swap;
  swap((Material&)first, (Material&)second);
}

void dg::UVMaterial::Use() const {
  Material::Use();
}

