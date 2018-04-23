//
//  scenes/RaytraceScene.h
//

#include <scenes/RaytraceScene.h>

#include <behaviors/KeyboardCameraController.h>
#include <behaviors/KeyboardLightController.h>
#include <materials/ScreenQuadMaterial.h>
#include <raytracing/Renderer.h>
#include <raytracing/TraceableCheckerboardMaterial.h>
#include <raytracing/TraceableModel.h>
#include <raytracing/TraceableStandardMaterial.h>
#include <raytracing/TraceableUVMaterial.h>
#include <glm/glm.hpp>

std::unique_ptr<dg::RaytraceScene> dg::RaytraceScene::Make() {
  return std::unique_ptr<dg::RaytraceScene>(new dg::RaytraceScene());
}

dg::RaytraceScene::RaytraceScene() : Scene() {}

void dg::RaytraceScene::Initialize() {
  // Lock window cursor to center.
  window->LockCursor();

  // Create skybox.
  //skybox = std::unique_ptr<Skybox>(new Skybox(std::make_shared<Texture>(
          //Texture::FromPath("assets/textures/skybox_daylight.png"))));

  // Create directinal light.
  auto directionalLight = std::make_shared<DirectionalLight>(
      glm::vec3(1.0f, 0.93f, 0.86f), 0, 0.8, 0.968f);
  directionalLight->LookAtDirection(glm::normalize(glm::vec3(-0.6, -1, -0.3)));
  //directionalLight->LookAtDirection(glm::normalize(glm::vec3(0.4, -1, -1.1)));
  AddChild(directionalLight);

  Behavior::Attach(directionalLight,
                   std::make_shared<KeyboardLightController>(window));

  // Create floor material.
  glm::vec2 floorSize(12, 28);
  TraceableCheckerboardMaterial floorMaterial;
  floorMaterial.SetSize(floorSize);
  floorMaterial.SetReflection(0.1f);
  floorMaterial.SetColorB(glm::vec3(1, 1, 0));
  floorMaterial.SetColorB(glm::vec3(1, 1, 0));

  // Create floor plane.
  AddChild(std::make_shared<TraceableModel>(
        dg::Mesh::Quad,
        std::make_shared<TraceableCheckerboardMaterial>(floorMaterial),
        Transform::RS(
          glm::quat(glm::radians(glm::vec3(-90, 0, 0))),
          glm::vec3(floorSize.x, floorSize.y, 1))));

  // Create sphere material.
  TraceableStandardMaterial sphereMaterial =
      TraceableStandardMaterial::WithColor(glm::vec3(0));
  sphereMaterial.SetSpecular(0.3);
  sphereMaterial.SetShininess(64);

  // Create front sphere.
  sphereMaterial.SetDiffuse(glm::vec3(0.025));
  sphereMaterial.SetSpecular(glm::vec3(1));
  sphereMaterial.SetTransmission(1);
  sphereMaterial.SetReflection(0);
  sphereMaterial.refractiveIndex = 1.52f;
  //sphereMaterial.refractiveIndex = 1.09f;
  sphereMaterial.SetRayBlending(RayBlendMode::Additive);
  AddChild(std::make_shared<TraceableModel>(
      dg::Mesh::Sphere,
      std::make_shared<TraceableStandardMaterial>(sphereMaterial),
      //Transform::TS(glm::vec3(-3, 2, 6), glm::vec3(2.5))));
      Transform::TS(glm::vec3(-3, 2, 0), glm::vec3(2.5))));

  // Create back sphere.
  sphereMaterial.SetDiffuse(glm::vec3(0.1));
  sphereMaterial.SetSpecular(glm::vec3(1));
  sphereMaterial.SetTransmission(0);
  sphereMaterial.SetReflection(1);
  sphereMaterial.SetRayBlending(RayBlendMode::Additive);
  AddChild(std::make_shared<TraceableModel>(
      dg::Mesh::Sphere,
      std::make_shared<TraceableStandardMaterial>(sphereMaterial),
      Transform::TS(glm::vec3(-1, 1.5, -2), glm::vec3(2.5))));

  // Create camera.
  mainCamera = std::make_shared<Camera>();
  mainCamera->transform.translation = glm::vec3(-3, 2, 5);
  mainCamera->LookAtPoint(glm::vec3(-3, 0, -7));
  AddChild(mainCamera);

  // Allow camera to be controller by the keyboard and mouse.
  Behavior::Attach(
      mainCamera,
      std::make_shared<KeyboardCameraController>(window, 4));

  // Material for rendering render canvas to screen.
  quadMaterial = std::make_shared<ScreenQuadMaterial>(
      glm::vec3(1), glm::vec2(2));
}

void dg::RaytraceScene::Update() {
  if (raytraceNextFrame) {
    raytraceNextFrame = false;
    Raytrace(); // NOTE: Extremely blocking.
    showRender = true;
  }

  if (showRender) {
    window->SetTitle("Rendered Scene");
    if (window->IsKeyJustPressed(Key::SPACE)) {
      showRender = false;
    }
    return;
  } else {
    window->SetTitle("Real-Time Preview (OpenGL)");
  }

  Scene::Update();

  if (window->IsKeyJustPressed(Key::SPACE)) {
    if (mainCamera->transform == renderCameraTransform) {
      showRender = true;
    } else {
      window->SetTitle("Rendering...");
      raytraceNextFrame = true;
    }
  }
}

void dg::RaytraceScene::RenderFrame() {
  if (showRender) {
    window->ResetViewport();

    ClearBuffer();
    ConfigureBuffer();

    quadMaterial->Use();
    Mesh::Quad->Draw();

    return;
  }

  Scene::RenderFrame();
}

void dg::RaytraceScene::ClearBuffer() {
  glm::vec3 background = glm::vec3(115, 163, 225) / 255.f;
  glClearColor(background.r, background.g, background.b, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void dg::RaytraceScene::ConfigureBuffer() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);
  glDisable(GL_CULL_FACE);
}

void dg::RaytraceScene::Raytrace() {
  window->UnlockCursor();
  std::unique_ptr<Renderer> renderer = std::unique_ptr<Renderer>(
      new Renderer(window->GetWidth(), window->GetHeight(), this));
  renderer->Render();
  quadMaterial->SetTexture(renderer->GetTexture());
  renderCameraTransform = mainCamera->transform;
}

bool dg::RaytraceScene::AutomaticWindowTitle() const {
  return false;
}
