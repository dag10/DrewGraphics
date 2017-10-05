//
//  Model.cpp
//

#include <Model.h>

dg::Model::Model() : SceneObject() {}

dg::Model::Model(
    std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material,
    Transform transform) : SceneObject(transform) {
  this->mesh = mesh;
  this->material = material;
}

dg::Model::Model(Model& other) {
  (SceneObject&)*this = SceneObject(other);
  this->mesh = other.mesh;
  this->material = other.material;
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
  swap((SceneObject&)first, (SceneObject&)second);
  swap(first.mesh, second.mesh);
  swap(first.material, second.material);
}

void dg::Model::Draw(glm::mat4x4 view, glm::mat4x4 projection) const {
  material->SetMatrixNormal(
      glm::mat3x3(glm::transpose(transform.Inverse().ToMat4())));
  material->SetMatrixM(transform.ToMat4());
  material->SetMatrixMVP(projection * view * transform);

  material->Use();

  mesh->Use();
  mesh->Draw();
  mesh->FinishUsing();
}

