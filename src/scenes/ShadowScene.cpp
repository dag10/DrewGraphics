//
//  scenes/ShadowScene.cpp
//

#include "dg/scenes/ShadowScene.h"
#include <forward_list>
#include <glm/glm.hpp>
#include "dg/Camera.h"
#include "dg/EngineTime.h"
#include "dg/Graphics.h"
#include "dg/Lights.h"
#include "dg/Mesh.h"
#include "dg/Model.h"
#include "dg/Shader.h"
#include "dg/Texture.h"
#include "dg/Window.h"
#include "dg/behaviors/KeyboardCameraController.h"
#include "dg/behaviors/KeyboardLightController.h"
#include "dg/materials/StandardMaterial.h"

std::unique_ptr<dg::ShadowScene> dg::ShadowScene::Make() {
  return std::unique_ptr<dg::ShadowScene>(new dg::ShadowScene());
}

dg::ShadowScene::ShadowScene() : Scene() {}

void dg::ShadowScene::Initialize() {
  Scene::Initialize();

  // Lock window cursor to center.
  window->LockCursor();

  // Create textures.
  std::shared_ptr<Texture> crateTexture =
      Texture::FromPath("assets/textures/container2.png");
  std::shared_ptr<Texture> crateSpecularTexture =
      Texture::FromPath("assets/textures/container2_specular.png");
  std::shared_ptr<Texture> hardwoodTexture =
      Texture::FromPath("assets/textures/hardwood.jpg");

  // Create ceiling light source.
  auto ceilingLight = std::make_shared<SpotLight>(glm::vec3(1.0f, 0.93f, 0.86f),
                                                  0.31, 0.91, 0.86);
  ceilingLight->transform.translation = glm::vec3(1.4f, 1.2f, -0.7f);
  ceilingLight->LookAtPoint({0, 0, 0});
  AddChild(ceilingLight);

  // Make light controllable with keyboard.
  Behavior::Attach(ceilingLight,
                   std::make_shared<KeyboardLightController>(window));

  // Create light cone material.
  StandardMaterial lightMaterial =
      StandardMaterial::WithColor(ceilingLight->GetSpecular());
  lightMaterial.SetLit(false);

  // Create light cone.
  auto lightModel = std::make_shared<Model>(
      Mesh::LoadOBJ("assets/models/cone.obj"),
      std::make_shared<StandardMaterial>(lightMaterial),
      Transform::RS(glm::quat(glm::radians(glm::vec3(90, 0, 0))),
                    glm::vec3(0.05f)));
  ceilingLight->AddChild(lightModel, false);

  // Create wooden cube material.
  StandardMaterial cubeMaterial = StandardMaterial::WithTexture(crateTexture);
  cubeMaterial.SetSpecular(crateSpecularTexture);
  cubeMaterial.SetShininess(64);

  // Create wooden cube.
  auto cube = std::make_shared<Model>(
      dg::Mesh::Cube, std::make_shared<StandardMaterial>(cubeMaterial),
      Transform::TS(glm::vec3(0, 0.25f, 0), glm::vec3(0.5f)));
  AddChild(cube);

  // Create floor material.
  const int floorSize = 500;
  StandardMaterial floorMaterial =
      StandardMaterial::WithTexture(hardwoodTexture);
  floorMaterial.SetUVScale(glm::vec2((float)floorSize));

  // Create floor plane.
  AddChild(std::make_shared<Model>(
      dg::Mesh::Quad, std::make_shared<StandardMaterial>(floorMaterial),
      Transform::RS(glm::quat(glm::radians(glm::vec3(-90, 0, 0))),
                    glm::vec3(floorSize, floorSize, 1))));

  // Configure camera.
  mainCamera->transform = Transform::T({1.054, 1.467, 2.048});
  mainCamera->LookAtDirection({-0.3126, -0.4692, -0.8259});

  // Allow camera to be controller by the keyboard and mouse.
  Behavior::Attach(mainCamera,
                   std::make_shared<KeyboardCameraController>(window));
}

void dg::ShadowScene::ClearBuffer() {
  Graphics::Instance->Clear(glm::vec3(0) / 255.f);
}
