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
  this->layer = other.layer;
}

void dg::Model::Draw(glm::mat4x4 view, glm::mat4x4 projection,
                     std::shared_ptr<Material> material) const {
  if (material == nullptr) {
    material = this->material;
  }

  glm::mat4x4 xfMat = CachedSceneSpace().ToMat4();

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
