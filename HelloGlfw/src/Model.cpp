//
//  Model.cpp
//

#include "Model.h"

dg::Model::Model(
    std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material,
    Transform transform) {
  this->mesh = mesh;
  this->material = material;
  this->transform = transform;
}

dg::Model::Model(Model& other) {
  this->mesh = other.mesh;
  this->material = other.material;
  this->transform = other.transform;
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
  swap(first.material, second.material);
  swap(first.transform, second.transform);
}

void dg::Model::Draw(glm::mat4x4 view, glm::mat4x4 projection) const {
  material->Use();

  material->shader->SetMat3(
      "MATRIX_NORMAL",
      glm::mat3x3(glm::transpose(transform.Inverse().ToMat4())));
  material->shader->SetMat4("MATRIX_M", transform);
  material->shader->SetMat4("MATRIX_MVP", projection * view * transform);

  mesh->Use();
  mesh->Draw();
  mesh->FinishUsing();
}

