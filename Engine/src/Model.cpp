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
                     Material *material) const {
  DrawContext context;
  context.view = view;
  context.projection = projection;
  Draw(context, material);
}

void dg::Model::Draw(const DrawContext &context, Material *material) const {
  if (material == nullptr) {
    material = this->material.get();
  }

  glm::mat4x4 xfMat = CachedSceneSpace().ToMat4();

  if (material->rasterizerOverride.HasDeclaredAttributes()) {
    Graphics::Instance->PushRasterizerState(material->rasterizerOverride);
  }

#if defined(_OPENGL)
  material->Use();
#endif

  if (context.cameraPos != nullptr) {
    material->SendCameraPosition(*context.cameraPos);
  }

  if (context.lights != nullptr) {
    material->SendLights(*context.lights);
  }

  if (context.shadowMap != nullptr) {
    material->SendShadowMap(context.shadowMap);
  }

  material->SendBufferDimensions(Graphics::Instance->GetViewportDimensions());
  material->SendMatrixNormal(glm::transpose(glm::inverse(xfMat)));
  material->SendMatrixM(xfMat);
  material->SendMatrixV(context.view);
  material->SendMatrixP(context.projection);
  material->SendMatrixMVP(context.projection * context.view * xfMat);

#if defined(_DIRECTX)
  material->Use();
#endif

  mesh->Draw();

  if (material->rasterizerOverride.HasDeclaredAttributes()) {
    Graphics::Instance->PopRasterizerState();
  }
}
