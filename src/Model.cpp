//
//  Model.cpp
//

#include "dg/Model.h"
#include "dg/Graphics.h"

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
  glm::mat4x4 xfMat = SceneSpace().ToMat4();

  if (material->rasterizerOverride.HasDeclaredAttributes()) {
    Graphics::Instance->PushRasterizerState(material->rasterizerOverride);
  }

#if defined(_OPENGL)
  material->Use();
#endif

  material->SendMatrixNormal(glm::transpose(glm::inverse(xfMat)));
  material->SendMatrixM(xfMat);
  material->SendMatrixMVP(projection * view * xfMat);

#if defined(_DIRECTX)
  material->Use();
#endif

  mesh->Draw();

  if (material->rasterizerOverride.HasDeclaredAttributes()) {
    Graphics::Instance->PopRasterizerState();
  }
}
