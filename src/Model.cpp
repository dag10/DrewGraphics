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

dg::Model::Model(Model& other) : SceneObject(other) {
  this->mesh = other.mesh;
  this->material = other.material;
}

void dg::Model::Draw(glm::mat4x4 view, glm::mat4x4 projection) const {
  Transform xf = SceneSpace();

  material->SetMatrixNormal(
      glm::mat3x3(glm::transpose(xf.Inverse().ToMat4())));
  material->SetMatrixM(xf.ToMat4());
  material->SetMatrixMVP(projection * view * xf);

  material->Use();

  mesh->Use();
  mesh->Draw();
  mesh->FinishUsing();
}

