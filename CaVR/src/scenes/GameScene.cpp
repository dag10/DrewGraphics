//
//  scenes/GameScene.cpp
//

#include "cavr/scenes/GameScene.h"
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include "cavr/CavrEngine.h"
#include "cavr/behaviors/CaveBehavior.h"
#include "dg/Camera.h"
#include "dg/Engine.h"
#include "dg/InputCodes.h"
#include "dg/Lights.h"
#include "dg/Model.h"
#include "dg/Skybox.h"
#include "dg/Texture.h"
#include "dg/Transform.h"
#include "dg/Window.h"
#include "dg/behaviors/KeyboardCameraController.h"
#include "dg/behaviors/KeyboardLightController.h"
#include "dg/materials/StandardMaterial.h"
#include "dg/vr/VRControllerState.h"
#include "dg/vr/VRManager.h"
#include "dg/vr/VRRenderModel.h"
#include "dg/vr/VRTrackedObject.h"

std::unique_ptr<cavr::GameScene> cavr::GameScene::Make() {
  return std::unique_ptr<cavr::GameScene>(new cavr::GameScene());
}

cavr::GameScene::GameScene() : dg::Scene() {
  //vr.requested = true;
}

void cavr::GameScene::Initialize() {
  dg::Scene::Initialize();

  // Make near clip plane shorter.
  cameras.main->nearClip = 0.01f;

  // Create skybox.
  // Image generated from
  // http://wwwtyro.github.io/space-3d/#animationSpeed=0.9019276654012347&fov=76.5786603914366&nebulae=true&pointStars=true&resolution=2048&seed=2uj2ah7o9z00&stars=true&sun=true
  skybox = dg::Skybox::Create(
      dg::Texture::FromPath("assets/textures/stars_with_sun_skybox.png"));

  // Create sky light.
  skyLight = std::make_shared<dg::DirectionalLight>(
      glm::vec3(1.0f, 0.93f, 0.86f), 0.f, 1.1f, 0.647f);
  skyLight->LookAtDirection(glm::vec3(0.183381f, -0.767736f, 0.613965f));
  dg::Behavior::Attach(skyLight,
                       std::make_shared<dg::KeyboardLightController>(window));
  AddChild(skyLight);

  // Create moon light to shade shadow from sun.
  auto moonLight = std::make_shared<dg::DirectionalLight>(
      glm::vec3(1.0f, 0.93f, 0.86f), 0.f, 0.35f, 0.f);
  moonLight->LookAtDirection(-skyLight->transform.Forward());
  skyLight->AddChild(moonLight);

  // Create floor material.
  auto floorMaterial = std::make_shared<dg::StandardMaterial>(
      dg::StandardMaterial::WithTransparentColor(
          glm::vec4(glm::vec3(85, 43, 112) / 255.f, 0.4f)));
  floorMaterial->rasterizerOverride.SetCullMode(
      dg::RasterizerState::CullMode::OFF);

  // Create floor.
  const float floorThickness = 0.05f;
  floor = std::make_shared<dg::Model>(
      dg::Mesh::Cube, floorMaterial,
      dg::Transform::TS(glm::vec3(0, -floorThickness / 2, 0),
                        glm::vec3(2.34f, floorThickness, 1.8f)));
  AddChild(floor);

  // Add objects to follow OpenVR tracked devices.
  leftController = std::make_shared<SceneObject>();
  vr.container->AddChild(leftController);
  dg::Behavior::Attach(
      leftController,
      std::make_shared<dg::VRTrackedObject>(
          vr::ETrackedControllerRole::TrackedControllerRole_LeftHand));
  dg::Behavior::Attach(leftController, std::make_shared<dg::VRRenderModel>());
  dg::Behavior::Attach(leftController,
                       std::make_shared<dg::VRControllerState>());
  rightController = std::make_shared<SceneObject>();
  dg::Behavior::Attach(
      rightController,
      std::make_shared<dg::VRTrackedObject>(
          vr::ETrackedControllerRole::TrackedControllerRole_RightHand));
  dg::Behavior::Attach(rightController, std::make_shared<dg::VRRenderModel>());
  dg::Behavior::Attach(rightController,
                       std::make_shared<dg::VRControllerState>());
  vr.container->AddChild(rightController);

  // Create attachment point point for eventual spaceship.
  shipAttachment =
      std::make_shared<SceneObject>(dg::Transform::T(dg::FORWARD * 0.035f));
  rightController->AddChild(shipAttachment);

  // Attach point light to right controller.
  controllerLight = std::make_shared<dg::PointLight>(
      glm::vec3(1.0f, 0.93f, 0.86f), 0.f, 4.0f, 3.f);
  controllerLight->LookAtDirection(-skyLight->transform.Forward());
  controllerLight->SetLinear(1.5f);
  controllerLight->SetQuadratic(3.0f);
  shipAttachment->AddChild(controllerLight, false);

  // Attach sphere to left controller to represent point light.
  auto lightSphereColor = std::make_shared<dg::StandardMaterial>(
      dg::StandardMaterial::WithColor(controllerLight->GetDiffuse()));
  lightSphereColor->SetLit(false);
  auto lightSphere = std::make_shared<dg::Model>(
      dg::Mesh::Sphere, lightSphereColor, dg::Transform::S(glm::vec3(0.025f)));
  controllerLight->AddChild(lightSphere, false);

  // Create intersectionFramebuffer for rendering cave intersection test.
  const int fbRes = 128;
  shipIntersectionSubrender.type = Subrender::Type::MonoscopicFramebuffer;
  shipIntersectionSubrender.renderSkybox = false;
  shipIntersectionSubrender.framebuffer =
      dg::FrameBuffer::Create(fbRes, fbRes, false, false, true);
  shipIntersectionSubrender.camera = std::make_shared<dg::Camera>();
  shipIntersectionSubrender.camera->farClip = 0.025f;
  shipIntersectionSubrender.camera->nearClip =
      shipIntersectionSubrender.camera->farClip * 0.001f;
  shipIntersectionSubrender.layerMask = GameScene::LayerMask::CaveGeometry();
  shipIntersectionSubrender.material = std::make_shared<dg::StandardMaterial>(
      dg::StandardMaterial::WithColor(glm::vec3(1)));
  shipIntersectionSubrender.material->rasterizerOverride.SetCullMode(
      dg::RasterizerState::CullMode::OFF);
  std::static_pointer_cast<dg::StandardMaterial>(
      shipIntersectionSubrender.material)
      ->SetLit(false);
  shipAttachment->AddChild(shipIntersectionSubrender.camera, false);

  // Attach quad to sphere temporarily to visualize intersection rendering.
  auto renderQuadMat =
      std::make_shared<dg::StandardMaterial>(dg::StandardMaterial::WithTexture(
          shipIntersectionSubrender.framebuffer->GetColorTexture()));
  renderQuadMat->SetLit(false);
  auto renderQuad = std::make_shared<dg::Model>(
      dg::Mesh::Quad, renderQuadMat,
      dg::Transform::TS(glm::vec3(-0.11f, 0, 0), glm::vec3(0.08f)));
  shipAttachment->AddChild(renderQuad, false);

  // If VR could not enable, position the camera in a useful place for
  // development and make it controllable with keyboard and mouse.
  if (!vr.enabled) {
    window->LockCursor();
    cameras.main->transform =
        dg::Transform::T(glm::vec3(-0.905f, 1.951f, -1.63f));
    cameras.main->LookAtDirection(glm::vec3(0.259f, -0.729f, 0.633f));
    dg::Behavior::Attach(
        cameras.main, std::make_shared<dg::KeyboardCameraController>(window));

    // Attach controller light to camera.
    lightSphere->material = std::make_shared<dg::StandardMaterial>(
        dg::StandardMaterial::WithTransparentColor(
            glm::vec4(1.f, 1.f, 1.f, 0.1f)));
    std::static_pointer_cast<dg::StandardMaterial>(lightSphere->material)
        ->SetLit(false);
    shipAttachment->transform = dg::Transform::T(glm::vec3(0, 0, -0.1f));
    renderQuad->transform =
        dg::Transform::TS(glm::vec3(-0.05f, 0, 0), glm::vec3(0.04f));
    cameras.main->AddChild(shipAttachment, false);
  }

  // Create cave.
  cave = std::make_shared<dg::SceneObject>();
  auto caveBehavior =
      dg::Behavior::Attach(cave, std::make_shared<CaveBehavior>());
  caveBehavior->SetShowKnots(!vr.enabled);
  caveBehavior->SetShowWireframe(!vr.enabled);
  AddChild(cave);
}

void cavr::GameScene::Update() {
  Scene::Update();
  auto leftState = leftController->GetBehavior<dg::VRControllerState>();
  auto rightState = rightController->GetBehavior<dg::VRControllerState>();
  auto caveBehavior = cave->GetBehavior<CaveBehavior>();

  // Swap left and right controllers with TILDE key.
  if (window->IsKeyJustPressed(dg::Key::GRAVE_ACCENT)) {
    auto leftTrackedObj = leftController->GetBehavior<dg::VRTrackedObject>();
    auto rightTrackedObj = rightController->GetBehavior<dg::VRTrackedObject>();
    std::swap(leftTrackedObj->role, rightTrackedObj->role);
    std::swap(leftTrackedObj->deviceIndex, rightTrackedObj->deviceIndex);
  }

  // Toggle outer material type with M key or left MENU button.
  if (window->IsKeyJustPressed(dg::Key::M) ||
      leftState->IsButtonJustPressed(dg::VRControllerState::Button::MENU)) {
    caveBehavior->SetShowWireframe(!caveBehavior->GetShowWireframe());
  }

  // Toggle controller light with SPACE key or right TOUCHPAD button.
  if (window->IsKeyJustPressed(dg::Key::SPACE) ||
      rightState->IsButtonJustPressed(
          dg::VRControllerState::Button::TOUCHPAD)) {
    controllerLight->enabled = !controllerLight->enabled;
  }

  // Toggle skybox and ground with B key or left GRIP buttons.
  if (window->IsKeyJustPressed(dg::Key::B) ||
      leftState->IsButtonJustPressed(dg::VRControllerState::Button::GRIP)) {
    skybox->enabled = !skybox->enabled;
    floor->enabled = !floor->enabled;
  }

  // Toggle sky light with L key or left TOUCHPAD button.
  if (window->IsKeyJustPressed(dg::Key::L) ||
      leftState->IsButtonJustPressed(dg::VRControllerState::Button::TOUCHPAD)) {
    skyLight->enabled = !skyLight->enabled;
  }

  // Toggle knot visibility with K key or right MENU button.
  if (window->IsKeyJustPressed(dg::Key::K) ||
      rightState->IsButtonJustPressed(dg::VRControllerState::Button::MENU)) {
    caveBehavior->SetShowKnots(!caveBehavior->GetShowKnots());
  }

  // Add new cave segment with ENTER key or clicking left TRIGGER.
  if (window->IsKeyJustPressed(dg::Key::ENTER) ||
      leftState->IsButtonJustPressed(dg::VRControllerState::Button::TRIGGER)) {
    caveBehavior->AddNextCaveSegment();
  }

  // If left mouse or X key or right trigger is held down, add to cave velocity.
  const float minRightTrigger = 0.15f;
  float rightTrigger =
      rightState->GetAxis(dg::VRControllerState::Axis::TRIGGER).x;
  if (dg::Engine::Instance().GetWindow()->IsMouseButtonPressed(
          dg::BUTTON_LEFT) ||
      dg::Engine::Instance().GetWindow()->IsKeyPressed(dg::Key::X)) {
    rightTrigger = 0.2f;
  }
  if (rightTrigger > minRightTrigger) {
    float thrustAmount =
        (rightTrigger - minRightTrigger) / (1.f - minRightTrigger);
    glm::vec3 thrustDir = shipAttachment->SceneSpace().Forward();
    caveBehavior->AddVelocity(thrustDir * -thrustAmount * 0.02f);
  }

  // Brake with right mouse or backspace or Z right GRIP.
  if (dg::Engine::Instance().GetWindow()->IsMouseButtonPressed(
          dg::BUTTON_RIGHT) ||
      window->IsKeyPressed(dg::Key::Z) ||
      rightState->IsButtonPressed(dg::VRControllerState::Button::GRIP)) {
    glm::vec3 velo = caveBehavior->GetVelocity();
    velo -= velo * 5.0f * (float)dg::Time::Delta;
    if (velo.length() < 0.2f) {
      velo = glm::vec3(0);
    }
    caveBehavior->SetVelocity(velo);
  }
}

void cavr::GameScene::RenderFramebuffers() {
  // Render scene for intersectionFramebuffer.
  PerformSubrender(shipIntersectionSubrender);
}
