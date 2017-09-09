//
//  Model.cpp
//

#include "Model.h"

dg::Model::Model(
    std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader,
    std::shared_ptr<Texture> texture, glm::vec2 uvScale,
    Transform transform) {
  this->mesh = mesh;
  this->shader = shader;
  this->texture = texture;
  this->uvScale = uvScale;
  this->transform = transform;
}

dg::Model::Model(Model& other) {
  this->mesh = other.mesh;
  this->shader = other.shader;
  this->texture = other.texture;
  this->uvScale = other.uvScale;
  this->transform = other.transform;
  this->invPortal = other.invPortal;
}

dg::Model::Model(Model&& other) {
  *this = std::move(other);
}

dg::Model& dg::Model::operator=(Model& other) {
  *this = Model(other);
  return *this;
}

dg::Model& dg::Model::operator=(Model&& other) {
  swap(*this, other);
  return *this;
}

void dg::swap(Model& first, Model& second) {
  using std::swap;
  swap(first.mesh, second.mesh);
  swap(first.shader, second.shader);
  swap(first.transform, second.transform);
  swap(first.texture, second.texture);
  swap(first.uvScale, second.uvScale);
  swap(first.invPortal, second.invPortal);
}

void dg::Model::Draw(glm::mat4x4 view, glm::mat4x4 projection) const {
  shader->Use();
  shader->SetMat4("MATRIX_M", transform);
  shader->SetMat4("MATRIX_MVP", projection * view * transform);
  shader->SetVec2("UVScale", uvScale);
  shader->SetMat4("InvPortal", invPortal);
  shader->SetTexture(0, "MainTex", *texture);
  mesh->Use();
  mesh->Draw();
  mesh->FinishUsing();
}

