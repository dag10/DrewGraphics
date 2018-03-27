//
//  Skybox.cpp
//

#include "dg/Skybox.h"
#include "dg/Graphics.h"
#include "dg/Mesh.h"
#include "dg/RasterizerState.h"
#include "dg/materials/StandardMaterial.h"

std::shared_ptr<dg::Skybox> dg::Skybox::Create(
    std::shared_ptr<Texture> texture) {
  return std::shared_ptr<Skybox>(new Skybox(texture));
}

dg::Skybox::Skybox(std::shared_ptr<Texture> texture) {
  StandardMaterial material = StandardMaterial::WithTexture(texture);
  material.SetLit(false);
  material.rasterizerOverride.SetCullMode(RasterizerState::CullMode::FRONT);
  material.rasterizerOverride.SetWriteDepth(false);

  model.material = std::make_shared<StandardMaterial>(material);
  model.mesh = Mesh::MappedCube;
}

void dg::Skybox::SetTexture(std::shared_ptr<Texture> texture) {
  std::static_pointer_cast<StandardMaterial>(model.material)
      ->SetDiffuse(texture);
}

void dg::Skybox::Draw(const Camera& camera) {
  glm::mat4x4 projection = camera.GetProjectionMatrix();
  Draw(camera, projection);
}

void dg::Skybox::Draw(const Camera& camera, vr::EVREye eye) {
  glm::mat4x4 projection = camera.GetProjectionMatrix(eye);
  Draw(camera, projection);
}

void dg::Skybox::Draw(const Camera& camera, glm::mat4x4 projection) {
  glm::mat4x4 view = camera.GetViewMatrix();

  // Remove translation component from view transform.
  view[3][0] = view[3][1] = view[3][2] = 0;

  model.Draw(view, projection);
}
