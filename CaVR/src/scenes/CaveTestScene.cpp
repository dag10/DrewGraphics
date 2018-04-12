//
//  scenes/CaveTestScene.cpp
//

#include "cavr/scenes/CaveTestScene.h"
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include "cavr/CavrEngine.h"
#include "dg/Lights.h"
#include "dg/Model.h"
#include "dg/Skybox.h"
#include "dg/Texture.h"
#include "dg/Transform.h"
#include "dg/behaviors/KeyboardCameraController.h"
#include "dg/behaviors/KeyboardLightController.h"
#include "dg/materials/StandardMaterial.h"
#include "dg/vr/VRManager.h"
#include "dg/vr/VRRenderModel.h"
#include "dg/vr/VRTrackedObject.h"

std::unique_ptr<cavr::CaveTestScene> cavr::CaveTestScene::Make() {
  return std::unique_ptr<cavr::CaveTestScene>(new cavr::CaveTestScene());
}

cavr::CaveTestScene::CaveTestScene() : dg::Scene() {
  //enableVR = true;
}

void cavr::CaveTestScene::Initialize() {
  dg::Scene::Initialize();

  // Make near clip plane shorter.
  mainCamera->nearClip = 0.01f;

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
  auto leftController = std::make_shared<SceneObject>();
  vrContainer->AddChild(leftController);
  dg::Behavior::Attach(
      leftController,
      std::make_shared<dg::VRTrackedObject>(
          vr::ETrackedControllerRole::TrackedControllerRole_LeftHand));
  dg::Behavior::Attach(leftController, std::make_shared<dg::VRRenderModel>());
  auto rightController = std::make_shared<SceneObject>();
  dg::Behavior::Attach(
      rightController,
      std::make_shared<dg::VRTrackedObject>(
          vr::ETrackedControllerRole::TrackedControllerRole_RightHand));
  dg::Behavior::Attach(rightController, std::make_shared<dg::VRRenderModel>());
  vrContainer->AddChild(rightController);

  // Attach point light to left controller.
  controllerLight = std::make_shared<dg::PointLight>(
      glm::vec3(1.0f, 0.93f, 0.86f), 0.f, 4.0f, 3.f);
  controllerLight->transform = dg::Transform::T(dg::FORWARD * 0.035f);
  controllerLight->LookAtDirection(-skyLight->transform.Forward());
  controllerLight->SetLinear(1.5f);
  controllerLight->SetQuadratic(3.0f);
  leftController->AddChild(controllerLight, false);

  // Attach sphere to left controller to represent point light.
  auto lightSphereColor = std::make_shared<dg::StandardMaterial>(
      dg::StandardMaterial::WithColor(controllerLight->GetDiffuse()));
  lightSphereColor->SetLit(false);
  auto lightSphere = std::make_shared<dg::Model>(
      dg::Mesh::Sphere, lightSphereColor, dg::Transform::S(glm::vec3(0.025f)));
  controllerLight->AddChild(lightSphere, false);

  // If VR could not enable, position the camera in a useful place for
  // development and make it controllable with keyboard and mouse.
  if (!enableVR) {
    window->LockCursor();
    mainCamera->transform =
        dg::Transform::T(glm::vec3(-0.905f, 1.951f, -1.63f));
    mainCamera->LookAtDirection(glm::vec3(0.259f, -0.729f, 0.633f));
    dg::Behavior::Attach(
        mainCamera, std::make_shared<dg::KeyboardCameraController>(window));

    // Attach controller light to camera.
    lightSphere->material = std::make_shared<dg::StandardMaterial>(
        dg::StandardMaterial::WithTransparentColor(
            glm::vec4(1.f, 1.f, 1.f, 0.1f)));
    std::static_pointer_cast<dg::StandardMaterial>(lightSphere->material)
        ->SetLit(false);
    controllerLight->transform = dg::Transform::T((dg::FORWARD * 0.2f));
    mainCamera->AddChild(controllerLight, false);
  }

  // Create knot material for visualization.
  knotDiskMaterial = std::make_shared<dg::StandardMaterial>(
      dg::StandardMaterial::WithTransparentColor(
          glm::vec4(0.5f, 0.1f, 0.1f, 0.4f)));

  // Create knot forward vector material for visualization.
  knotArrowMaterial = std::make_shared<dg::StandardMaterial>(
      dg::StandardMaterial::WithColor(glm::vec4(0.7f, 0.1f, 0.1f, 0.4f)));

  // Create knot vertex material for visualization.
  ringVertexMaterial = std::make_shared<dg::StandardMaterial>(
      dg::StandardMaterial::WithTransparentColor(
          glm::vec4(0.4f, 0.4f, 0.9f, 0.9f)));
  ringVertexMaterial->rasterizerOverride =
      dg::RasterizerState::AdditiveBlending();

  // Create inner cave material.
  glm::vec3 caveColor = glm::vec3(0.11f);
  caveMaterial = std::make_shared<dg::StandardMaterial>(
      dg::StandardMaterial::WithColor(caveColor));

  // Create outer transparent cave material.
  caveTransparentMaterial = std::make_shared<dg::StandardMaterial>(
      dg::StandardMaterial::WithTransparentColor(glm::vec4(caveColor, 0.3f)));
  caveTransparentMaterial->rasterizerOverride.SetCullMode(
      dg::RasterizerState::CullMode::FRONT);

  // Create outer wireframe cave material.
  caveWireframeMaterial = std::make_shared<dg::StandardMaterial>(
      dg::StandardMaterial::WithWireframeColor(
          glm::vec4(0.1f, 0.5f, 0.1f, 0.3f)));
  caveWireframeMaterial->rasterizerOverride.SetCullMode(
      dg::RasterizerState::CullMode::FRONT);

  // Create cave containers.
  caveContainer = std::make_shared<dg::SceneObject>(
      dg::Transform::R(glm::quat(glm::radians(glm::vec3(0, 180, 0)))));
  AddChild(caveContainer);
  knots = std::make_shared<dg::SceneObject>();
  caveContainer->AddChild(knots, false);

  // Join multiple ArcSegments together.
  CaveSegment::KnotSet arcKnots = CreateArcKnots().WithInterpolatedKnots();
  CaveSegment currentArcSegment(arcKnots.TransformedBy(
      dg::Transform::TS(glm::vec3(0.5, 0, -1), glm::vec3(0.8f))));
  CaveSegment originalSegment = currentArcSegment;
  AddCaveSegment(currentArcSegment);
  srand(1);
  for (int i = 1; i < 20; i++) {
    float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    arcKnots.knots.front()->RotateBy(glm::radians(r * 360.f));
    //arcKnots.knots.front()->RotateBy(glm::radians(180.f));
    currentArcSegment = CaveSegment(arcKnots, currentArcSegment);
    AddCaveSegment(currentArcSegment);
  }
  currentArcSegment = originalSegment;
}

cavr::CaveSegment::KnotSet cavr::CaveTestScene::CreateArcKnots() {
  CaveSegment::KnotSet set;

  const glm::vec3 start(1.f, 0.8f, 0.4f);
  const glm::vec3 middle(-0.1f, 1.3f, -0.2f);
  const glm::vec3 end(-1.f, 0.75f, 0.4f);
  const float radius = 0.15f;
  set.knots.push_back(std::shared_ptr<CaveSegment::Knot>(new CaveSegment::Knot(
      start, glm::normalize(glm::vec3(-0.1f, 0.4f, -0.5f)), radius, 15.f)));
  set.knots.push_back(std::shared_ptr<CaveSegment::Knot>(new CaveSegment::Knot(
      middle, glm::normalize(glm::vec3(-1, 0, 0)), radius * 0.9f, 15.f)));
  set.knots.push_back(std::shared_ptr<CaveSegment::Knot>(new CaveSegment::Knot(
      end, glm::normalize(glm::vec3(-1.f, 0.f, 0.3f)), radius, 15.f)));

  return set;
}

cavr::CaveSegment::KnotSet cavr::CaveTestScene::CreateStraightKnots() {
  CaveSegment::KnotSet set;

  const glm::vec3 start(1.f, 0.75f, 0.f);
  const glm::vec3 end(-1.f, 0.75f, 0.f);
  const float radius = 0.1f;
  set.knots.push_back(std::shared_ptr<CaveSegment::Knot>(
      new CaveSegment::Knot(start, -dg::RIGHT, radius, 40.f)));
  set.knots.push_back(std::shared_ptr<CaveSegment::Knot>(
      new CaveSegment::Knot(end, -dg::RIGHT, radius * 0.1f, 40.f)));

  return set;
}

cavr::CaveSegment::KnotSet cavr::CaveTestScene::CreateVerticalKnots() {
  CaveSegment::KnotSet set;

  const glm::vec3 start(-0.5f, 0.f, -0.8f);
  const glm::vec3 end(-0.5f, 3.f, -0.8f);
  const float radius = 0.1f;
  const float speed = 24.f;
  set.knots.push_back(std::shared_ptr<CaveSegment::Knot>(new CaveSegment::Knot(
      dg::Transform::TRS(start, glm::quat(glm::radians(glm::vec3(90, 180, 0))),
                         glm::vec3(radius)),
      speed)));
  set.knots.push_back(std::shared_ptr<CaveSegment::Knot>(new CaveSegment::Knot(
      dg::Transform::TRS(end, glm::quat(glm::radians(glm::vec3(90, 0, 0))),
                         glm::vec3(radius)),
      speed)));

  return set;
}

void cavr::CaveTestScene::AddCaveSegment(const CaveSegment &segment) {
  // Visualize each original knots as models.
  auto interpolated =
      !segment.GetOriginalKnotSet().noninterpolatedKnots.empty();
  for (auto &knot : interpolated
                        ? segment.GetOriginalKnotSet().noninterpolatedKnots
                        : segment.GetOriginalKnotSet().knots) {
    knots->AddChild(CreateKnotVisualization(*knot), false);
  }

  // Create mesh for tunnel.
  caveContainer->AddChild(std::make_shared<dg::Model>(
                              segment.GetMesh(), caveMaterial, dg::Transform()),
                          false);
  auto outerMaterial =
      enableVR ? caveTransparentMaterial : caveWireframeMaterial;
  caveContainer->AddChild(
      std::make_shared<dg::Model>(segment.GetMesh(), outerMaterial,
                                  dg::Transform()),
      false);
}

std::shared_ptr<dg::SceneObject>
cavr::CaveTestScene::CreateKnotVertexVisualization(
    const CaveSegment::Knot &knot) const {
  auto container = std::make_shared<SceneObject>();
  for (int i = 0; i < CaveSegment::VerticesPerRing; i++) {
    container->AddChild(
        std::make_shared<dg::Model>(
            dg::Mesh::Sphere, ringVertexMaterial,
            dg::Transform::TS(knot.GetVertexPosition(i), glm::vec3(0.002f))),
        false);
  }
  return container;
}

std::shared_ptr<dg::SceneObject> cavr::CaveTestScene::CreateKnotVisualization(
    const CaveSegment::Knot &knot) const {
  auto container = std::make_shared<SceneObject>();

  // Disk
  float diskWidth = 0.1f;
  container->AddChild(
      std::make_shared<dg::Model>(
          dg::Mesh::Cylinder, knotDiskMaterial,
          knot.GetXF() *
              dg::Transform::RS(glm::quat(glm::radians(glm::vec3(-90, 0, 0))),
                                glm::vec3(2.f, diskWidth, 2.f))),
      false);

  // Arrow stem
  float stemWidth = 0.07f;
  float stemHeight = knot.GetCurveSpeed() * 0.1f;
  auto stem = std::make_shared<dg::Model>(
      dg::Mesh::Cylinder, knotArrowMaterial,
      knot.GetXF() *
          dg::Transform::TRS(dg::FORWARD * stemHeight * 0.5f,
                             glm::quat(glm::radians(glm::vec3(-90, 0, 0))),
                             glm::vec3(stemWidth, stemHeight, stemWidth)));
  container->AddChild(stem, false);

  // Arrow cone
  float arrowWidth = 2.5f * stemWidth;
  float arrowHeight = 1.0f * arrowWidth;
  auto cone = std::make_shared<dg::Model>(
      dg::Mesh::LoadOBJ("assets/models/cone.obj"), knotArrowMaterial,
      knot.GetXF() *
          dg::Transform::TRS(dg::FORWARD * (arrowHeight * 0.5f + stemHeight),
                             glm::quat(glm::radians(glm::vec3(-90, 0, 0))),
                             glm::vec3(arrowWidth, arrowHeight, arrowWidth)));
  container->AddChild(cone, false);

  // "Right" cone
  auto up = std::make_shared<dg::Model>(
      dg::Mesh::LoadOBJ("assets/models/cone.obj"), knotArrowMaterial,
      knot.GetXF() *
          dg::Transform::TRS(dg::RIGHT * 1.0f,
                             glm::quat(glm::radians(glm::vec3(0, 0, -90))),
                             glm::vec3(diskWidth * 2.f)));
  container->AddChild(up, false);

  return container;
}

void cavr::CaveTestScene::Update() {
  Scene::Update();

  // Toggle controller light with SPACE key.
  if (window->IsKeyJustPressed(dg::Key::SPACE)) {
    controllerLight->enabled = !controllerLight->enabled;
  }

  // Toggle skybox and ground with B key.
  if (window->IsKeyJustPressed(dg::Key::B)) {
    skybox->enabled = !skybox->enabled;
    floor->enabled = !floor->enabled;
  }

  // Toggle sky light with L key.
  if (window->IsKeyJustPressed(dg::Key::L)) {
    skyLight->enabled = !skyLight->enabled;
  }

  // Toggle knot visibility with K key.
  if (window->IsKeyJustPressed(dg::Key::K)) {
    knots->enabled = !knots->enabled;
  }
}
