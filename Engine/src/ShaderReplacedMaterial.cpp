//
//  materials/ShaderReplacedMaterial.cpp
//

#include "dg/ShaderReplacedMaterial.h"

dg::ShaderReplacedMaterial::ShaderReplacedMaterial(
    std::shared_ptr<Material> material, std::shared_ptr<Shader> newShader)
    : Material(), material(material) {
  shader = newShader;
}

dg::ShaderReplacedMaterial::ShaderReplacedMaterial(
    ShaderReplacedMaterial &other)
    : Material(other) {}

dg::ShaderReplacedMaterial::ShaderReplacedMaterial(
    ShaderReplacedMaterial &&other) {
  *this = std::move(other);
}

dg::ShaderReplacedMaterial &dg::ShaderReplacedMaterial::operator=(
    ShaderReplacedMaterial &other) {
  *this = ShaderReplacedMaterial(other);
  return *this;
}

dg::ShaderReplacedMaterial &dg::ShaderReplacedMaterial::operator=(
    ShaderReplacedMaterial &&other) {
  swap(*this, other);
  return *this;
}

void dg::swap(ShaderReplacedMaterial &first, ShaderReplacedMaterial &second) {
  using std::swap;
  swap((Material &)first, (Material &)second);
  swap(first.material, second.material);
}

void dg::ShaderReplacedMaterial::SendShaderProperties() const {
  (*material).SendShaderProperties();
}
