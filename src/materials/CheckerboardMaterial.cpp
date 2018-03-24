//
//  materials/CheckerboardMaterial.cpp
//

#include <materials/CheckerboardMaterial.h>

std::shared_ptr<dg::Shader> dg::CheckerboardMaterial::checkerboardShader =
    nullptr;

dg::CheckerboardMaterial::CheckerboardMaterial() : Material() {
  if (checkerboardShader == nullptr) {
    checkerboardShader = std::make_shared<Shader>(
        dg::Shader::FromFiles("assets/shaders/checkerboard.v.glsl",
                              "assets/shaders/checkerboard.f.glsl"));
  }

  shader = CheckerboardMaterial::checkerboardShader;

  SetDefaultProperties();
}

dg::CheckerboardMaterial::CheckerboardMaterial(CheckerboardMaterial& other)
    : Material(other) {
}

dg::CheckerboardMaterial::CheckerboardMaterial(CheckerboardMaterial&& other) {
  *this = std::move(other);
}

dg::CheckerboardMaterial &dg::CheckerboardMaterial::operator=(
    CheckerboardMaterial &other) {
  *this = CheckerboardMaterial(other);
  return *this;
}

dg::CheckerboardMaterial &dg::CheckerboardMaterial::operator=(
    CheckerboardMaterial &&other) {
  swap(*this, other);
  return *this;
}

void dg::swap(CheckerboardMaterial& first, CheckerboardMaterial& second) {
  using std::swap;
  swap((Material&)first, (Material&)second);
}

void dg::CheckerboardMaterial::SetDefaultProperties() {
  SetSize(glm::vec2(4));
  SetColorA(glm::vec3(1, 0, 0));
  SetColorB(glm::vec3(1, 1, 0));
}

void dg::CheckerboardMaterial::Use() const {
  Material::Use();
}

void dg::CheckerboardMaterial::SetSize(glm::vec2 size) {
  SetProperty("_Size", size);
}

void dg::CheckerboardMaterial::SetColorA(glm::vec3 color) {
  SetProperty("_Colors[0]", color);
}

void dg::CheckerboardMaterial::SetColorB(glm::vec3 color) {
  SetProperty("_Colors[1]", color);
}
