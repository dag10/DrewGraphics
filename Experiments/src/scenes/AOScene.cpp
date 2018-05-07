//
//  scenes/AOScene.cpp
//

#include "dg/scenes/AOScene.h"

#include <forward_list>
#include <glm/glm.hpp>
#include "dg/Camera.h"
#include "dg/Graphics.h"
#include "dg/Lights.h"
#include "dg/Mesh.h"
#include "dg/Model.h"
#include "dg/Shader.h"
#include "dg/Skybox.h"
#include "dg/Texture.h"
#include "dg/Window.h"
#include "dg/behaviors/KeyboardCameraController.h"
#include "dg/behaviors/KeyboardLightController.h"
#include "dg/materials/ScreenQuadMaterial.h"
#include "dg/materials/StandardMaterial.h"
#include "dg/materials/UVMaterial.h"

std::unique_ptr<dg::AOScene> dg::AOScene::Make() {
  return std::unique_ptr<dg::AOScene>(new dg::AOScene());
}

dg::AOScene::AOScene() : Scene() {}

void dg::AOScene::Initialize() {
  Scene::Initialize();

  // Lock window cursor to center.
  window->LockCursor();

  // Create skybox.
  skybox =
      Skybox::Create(Texture::FromPath("assets/textures/skybox_daylight.png"));

  // Create primary sky light.
  auto primarySkyLight = std::make_shared<DirectionalLight>(
      glm::vec3(1.0f, 0.93f, 0.86f), 0.4, 0.82, 0.07);
  primarySkyLight->LookAtDirection(glm::normalize(glm::vec3(-0.3f, -1, -0.2f)));
  //Behavior::Attach(primarySkyLight,
                   //std::make_shared<KeyboardLightController>(window));
  AddChild(primarySkyLight);

  // Create secondary sky light.
  auto secondarySkyLight = std::make_shared<DirectionalLight>(
      primarySkyLight->GetDiffuse(), 0, 0.21, 0);
  secondarySkyLight->LookAtDirection(-primarySkyLight->transform.Forward());
  //Behavior::Attach(secondarySkyLight,
                   //std::make_shared<KeyboardLightController>(window));
  AddChild(secondarySkyLight);

  // Create camera point light.
  auto cameraLight =
      std::make_shared<PointLight>(primarySkyLight->GetDiffuse(), 0, 0.17, 0);
  cameras.main->AddChild(cameraLight, false);
  Behavior::Attach(cameraLight,
                   std::make_shared<KeyboardLightController>(window));

  // Create model material.
  StandardMaterial cubeMaterial = StandardMaterial::WithColor(glm::vec3(0.5));
  cubeMaterial.SetSpecular(glm::vec3(0));

  // Load model.
  AddChild(std::make_shared<Model>(
      Mesh::LoadOBJ("assets/models/crytek-sponza/sponza.obj"),
      std::make_shared<StandardMaterial>(cubeMaterial),
      //std::make_shared<UVMaterial>(),
      Transform::S(glm::vec3(0.0025))));

  // Create floor material.
  const int floorSize = 2;
  StandardMaterial floorMaterial = StandardMaterial::WithColor(glm::vec3(0.2f));
  floorMaterial.SetUVScale(glm::vec2((float)floorSize));
  floorMaterial.SetSpecular(glm::vec3(1));
  floorMaterial.SetShininess(64);

  // Configure camera.
  cameras.main->transform.translation = glm::vec3(-3.11, 1.75, 0.23);
  cameras.main->LookAtDirection(glm::vec3(0.9713, -0.198, -0.132));

  // Allow camera to be controller by the keyboard and mouse.
  Behavior::Attach(cameras.main,
                   std::make_shared<KeyboardCameraController>(window));

  // Create screen space quads to visualize internal textures.
  for (int i = 0; i < 3; i++) {
    auto quad = std::make_shared<Model>(
        Mesh::Quad,
        std::make_shared<ScreenQuadMaterial>(glm::vec3(1, 0, 1), glm::vec2(1)),
        Transform());
    overlayQuads.push_back(quad);
    AddChild(quad);
  }

  InitializeSubrenders();
}

void dg::AOScene::InitializeSubrenders() {
  geometrySubrender.type = Subrender::Type::MonoscopicWindow;
  geometrySubrender.renderSkybox = false;
}

void dg::AOScene::CreateGBuffer() {
  FrameBuffer::Options opts;
  opts.width = window->GetWidth();
  opts.height = window->GetHeight();
  opts.depthReadable = true;
  opts.hasColor = true;
  opts.hasStencil = false;
  geometrySubrender.framebuffer = FrameBuffer::Create(opts);
}

void dg::AOScene::PreRender() {
  // Always render geometry pass with whichever camera we're about to do
  // the main render with.
  geometrySubrender.camera = subrenders.main.camera;
}

void dg::AOScene::RenderFramebuffers() {
  // Create or resize geometry framebuffer if needed.
  if (geometrySubrender.framebuffer == nullptr ||
      geometrySubrender.framebuffer->GetWidth() != window->GetWidth() ||
      geometrySubrender.framebuffer->GetHeight() != window->GetHeight()) {
    CreateGBuffer();
  }

  PerformSubrender(geometrySubrender);
}

void dg::AOScene::Update() {
  Scene::Update();

  // G key switches to GBuffer overlay state.
  if (window->IsKeyJustPressed(Key::G)) {
    overlayState = OverlayState::GBuffer;
  }

  // N key switches to no overlay state.
  if (window->IsKeyJustPressed(Key::N)) {
    overlayState = OverlayState::None;
  }

  // Update overlay quads based on overlay state.
  for (auto &quad : overlayQuads) {
    quad->enabled = false;
  }
  switch (overlayState) {
    case OverlayState::None:
      break;
    case OverlayState::GBuffer: {
      glm::vec2 quadScale =
          glm::vec2(1) / glm::vec2(window->GetAspectRatio(), 1);
      overlayQuads[0]->enabled = true;
      std::static_pointer_cast<ScreenQuadMaterial>(overlayQuads[0]->material)
          ->SetScale(quadScale);
      std::static_pointer_cast<ScreenQuadMaterial>(overlayQuads[0]->material)
          ->SetOffset(glm::vec2(1 - quadScale.x * 0.5, quadScale.y * 0.5));
      //std::static_pointer_cast<ScreenQuadMaterial>(overlayQuads[0]->material)
          //->SetTexture(geometrySubrender.framebuffer->GetDepthTexture());
      break;
    };
  }
}
