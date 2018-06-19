//
//  Skybox.cpp
//

#include "dg/Skybox.h"
#include "dg/Graphics.h"
#include "dg/Material.h"
#include "dg/Mesh.h"
#include "dg/RasterizerState.h"
#include "dg/Shader.h"
#include "dg/materials/StandardMaterial.h"

#pragma region Base Skybox

std::shared_ptr<dg::Skybox> dg::Skybox::Create(
    std::shared_ptr<Texture> texture) {
  switch (texture->GetType()) {
    case TextureType::_2D:
      return std::shared_ptr<Skybox>(new CubeMeshSkybox(texture));
    case TextureType::CUBEMAP:
      return std::shared_ptr<Skybox>(new CubemapSkybox(texture));
  }
}

void dg::Skybox::Draw(const Camera& camera) {
  glm::mat4x4 projection = camera.GetProjectionMatrix();
  Draw(camera, projection);
}

void dg::Skybox::Draw(const Camera& camera, vr::EVREye eye) {
  glm::mat4x4 projection = camera.GetProjectionMatrix(eye);
  Draw(camera, projection);
}

#pragma endregion
#pragma region Cube Mesh Skybox

dg::CubeMeshSkybox::CubeMeshSkybox(std::shared_ptr<Texture> texture) {
  StandardMaterial material = StandardMaterial::WithTexture(texture);
  material.SetLit(false);
  material.rasterizerOverride.SetCullMode(RasterizerState::CullMode::FRONT);
  material.rasterizerOverride.SetWriteDepth(false);

  model.material = std::make_shared<StandardMaterial>(material);
  model.mesh = Mesh::MappedCube;
}

void dg::CubeMeshSkybox::Draw(const Camera& camera, glm::mat4x4 projection) {
  glm::mat4x4 view = camera.GetViewMatrix();

  model.transform.scale =
      glm::vec3((camera.nearClip + camera.farClip) * 0.5f) * 2.f;
  model.CacheSceneSpace();

  // Remove translation component from view transform.
  view[3][0] = view[3][1] = view[3][2] = 0;

  model.Draw(view, projection);
}

#pragma endregion
#pragma region Cubemap Skybox

dg::CubemapSkybox::CubemapSkybox(std::shared_ptr<Texture> cubemap) {
  model.material = std::make_shared<Material>();
  model.material->shader = Shader::FromFiles("assets/shaders/skybox.v.glsl",
                                             "assets/shaders/skybox.f.glsl");
  model.material->rasterizerOverride.SetWriteDepth(false);
  model.material->SetProperty("skybox", cubemap);
  model.mesh = Mesh::ScreenQuad;
}

void dg::CubemapSkybox::Draw(const Camera& camera, glm::mat4x4 projection) {
  glm::mat4x4 view = camera.GetViewMatrix();

  // Remove translation component from view transform.
  view[3][0] = view[3][1] = view[3][2] = 0;

  model.Draw(view, projection);
}

#pragma endregin
