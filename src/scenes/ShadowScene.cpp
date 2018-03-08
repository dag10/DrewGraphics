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
  spotlight = std::make_shared<SpotLight>(glm::vec3(1.0f, 0.93f, 0.86f), 0.31,
                                          0.91, 0.86);
  spotlight->transform.translation = glm::vec3(1.4f, 1.6f, -0.7f);
  spotlight->LookAtPoint({0, 0, 0});
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

  // Create frame buffer for light.
  //framebuffer = std::make_shared<FrameBuffer>(1024, 1024, true, false, false);
  framebuffer = std::make_shared<FrameBuffer>(1024, 1024, true, false, true);

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
  mainCamera->transform = Transform::T({1.6, 1.74, 2.38});
  mainCamera->LookAtDirection({-0.279, -0.465, -0.841});

  // Allow camera to be controller by the keyboard and mouse.
  Behavior::Attach(mainCamera,
                   std::make_shared<KeyboardCameraController>(window));

  // Material for drawing depth map over scene.
  quadMaterial =
      std::make_shared<ScreenQuadMaterial>(glm::vec3(0), glm::vec2(1));

  SetState(State::Spotlight);
}

void dg::ShadowScene::Update() {
  Scene::Update();

  // Slowly rotate light.
  spotlight->transform =
      Transform::R(glm::quat(glm::radians(glm::vec3(0, Time::Delta * 30, 0)))) *
      spotlight->transform;

  if (window->IsKeyJustPressed(Key::NUM_1)) {
    SetState(State::Spotlight);
  } else if (window->IsKeyJustPressed(Key::NUM_2)) {
    SetState(State::RenderLight);
  } else if (window->IsKeyJustPressed(Key::NUM_3)) {
    SetState(State::RenderDepth);
  } else if (window->IsKeyJustPressed(Key::NUM_4)) {
    SetState(State::Shadow);
  } else if (window->IsKeyJustPressed(Key::NUM_5)) {
    SetState(State::Feather);
  } else if (window->IsKeyJustPressed(Key::NUM_6)) {
    SetState(State::Robot);
  }
}

void dg::ShadowScene::SetState(State state) {
  this->state = state;
  spotlight->SetFeather((int)state >= (int)State::Feather ? glm::radians(5.f)
                                                          : 0.f);
}

void dg::ShadowScene::RenderFrame() {
#if defined(_OPENGL)

  // Render scene for light framebuffer.
  Camera lightCamera;
  lightCamera.transform = spotlight->SceneSpace();
  lightCamera.fov = spotlight->GetCutoff() * 2;
  lightCamera.nearClip = 0.5;
  lightCamera.farClip = 7;
  lightTransform =
      lightCamera.GetProjectionMatrix() * lightCamera.GetViewMatrix();
  framebuffer->Bind();
  framebuffer->SetViewport();

  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  DrawScene(lightCamera);

  framebuffer->Unbind();
  window->ResetViewport();

#endif

  ClearBuffer();

  // Render scene for real.
  mainCamera->aspectRatio = window->GetAspectRatio();
  DrawScene(*mainCamera);


  if ((int)state >= (int)State::RenderLight) {
    // Draw light color rendering over scene.
    glm::vec2 scale = glm::vec2(1) / glm::vec2(window->GetAspectRatio(), 1);
    quadMaterial->SetTexture(framebuffer->GetColorTexture());
    quadMaterial->SetRedChannelOnly(false);
    quadMaterial->SetScale(scale);
    quadMaterial->SetOffset(glm::vec2(1 - scale.x * 0.5, scale.y * 0.5));
    quadMaterial->Use();
    Mesh::Quad->Draw();
  }

  if ((int)state >= (int)State::RenderDepth) {
    // Draw depth map over scene.
    glm::vec2 scale = glm::vec2(1) / glm::vec2(window->GetAspectRatio(), 1);
    quadMaterial->SetTexture(framebuffer->GetDepthTexture());
    quadMaterial->SetScale(scale);
    quadMaterial->SetRedChannelOnly(true);
    quadMaterial->SetOffset(glm::vec2(1 - scale.x * 0.5, -1 + scale.y * 0.5));
    quadMaterial->Use();
    Mesh::Quad->Draw();
  }
}

void dg::ShadowScene::PrepareModelForDraw(
    const Model& model,
    glm::vec3 cameraPosition,
    glm::mat4x4 view,
    glm::mat4x4 projection,
    const Light::ShaderData(&lights)[Light::MAX_LIGHTS]) const {
  Scene::PrepareModelForDraw(model, cameraPosition, view, projection, lights);
  if ((int)state >= (int)State::Shadow) {
    model.material->SendShadowMap(framebuffer->GetDepthTexture());
    model.material->SendLightTransform(lightTransform);
  } else {
    model.material->SendLightTransform(glm::mat4(0));
  }
}
