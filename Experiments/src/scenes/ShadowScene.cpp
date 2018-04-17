//
//  scenes/ShadowScene.cpp
//

#include "dg/scenes/ShadowScene.h"
#include <forward_list>
#include <glm/glm.hpp>
#include "dg/Camera.h"
#include "dg/EngineTime.h"
#include "dg/FrameBuffer.h"
#include "dg/Graphics.h"
#include "dg/Lights.h"
#include "dg/Mesh.h"
#include "dg/Model.h"
#include "dg/Shader.h"
#include "dg/Texture.h"
#include "dg/Window.h"
#include "dg/behaviors/KeyboardCameraController.h"
#include "dg/behaviors/KeyboardLightController.h"
#include "dg/materials/ScreenQuadMaterial.h"
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
  spotlight = std::make_shared<SpotLight>(glm::vec3(1.0f, 0.93f, 0.86f), 0.31f,
                                          0.91f, 0.86f);
  spotlight->transform.translation = glm::vec3(1.4f, 1.2f, 0);
  spotlight->LookAtPoint({0, 0, 0});
  spotlight->SetCastShadows(true);
  AddChild(spotlight);

  // Make light controllable with keyboard.
  Behavior::Attach(spotlight,
                   std::make_shared<KeyboardLightController>(window));

  // Create light cone material.
  StandardMaterial lightMaterial =
      StandardMaterial::WithColor(spotlight->GetSpecular());
  lightMaterial.SetLit(false);

  // Create light cone.
  auto lightModel = std::make_shared<Model>(
      Mesh::LoadOBJ("assets/models/cone.obj"),
      std::make_shared<StandardMaterial>(lightMaterial),
      Transform::RS(glm::quat(glm::radians(glm::vec3(90, 0, 0))),
                    glm::vec3(0.05f)));
  spotlight->AddChild(lightModel, false);

  // Create wooden cube material.
  StandardMaterial cubeMaterial = StandardMaterial::WithTexture(crateTexture);
  cubeMaterial.SetSpecular(crateSpecularTexture);
  cubeMaterial.SetShininess(64);

  // Create wooden cube.
  cube = std::make_shared<Model>(
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
  cameras.main->transform = Transform::T({1.054, 1.467, 2.048});
  cameras.main->LookAtDirection({-0.3126, -0.4692, -0.8259});

  // Allow camera to be controller by the keyboard and mouse.
  Behavior::Attach(cameras.main,
                   std::make_shared<KeyboardCameraController>(window));

  // Material for drawing depth map over scene.
  quadMaterial =
      std::make_shared<ScreenQuadMaterial>(glm::vec3(0), glm::vec2(1));

  // Create custom shadow framebuffer with color map (for special screen
  // quad output).
  subrenders.light.framebuffer =
      FrameBuffer::Create(2048, 2048, true, false, true);
}

void dg::ShadowScene::Update() {
  Scene::Update();

  // Slowly rotate light.
  spotlight->transform =
      Transform::R(glm::quat(glm::radians(glm::vec3(0, Time::Delta * 30, 0)))) *
      spotlight->transform;
}

void dg::ShadowScene::RenderOverlays() {
  // Draw depth map over scene.
  quadMaterial->SetTexture(subrenders.light.framebuffer->GetDepthTexture());
  glm::vec2 scale = glm::vec2(1) / glm::vec2(window->GetAspectRatio(), 1);
  quadMaterial->SetScale(scale);
  quadMaterial->SetRedChannelOnly(true);
  quadMaterial->SetOffset(glm::vec2(1 - scale.x * 0.5, scale.y * 0.5));
  quadMaterial->Use();
  Mesh::Quad->Draw();

  // Draw light color rendering over scene.
  quadMaterial->SetTexture(subrenders.light.framebuffer->GetColorTexture());
  quadMaterial->SetRedChannelOnly(false);
  quadMaterial->SetScale(scale);
  quadMaterial->SetOffset(glm::vec2(1 - scale.x * 0.5, -1 + scale.y * 0.5));
  quadMaterial->Use();
  Mesh::Quad->Draw();
}
