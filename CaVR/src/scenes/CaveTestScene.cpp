//
//  scenes/CaveTestScene.cpp
//

#include "cavr/scenes/CaveTestScene.h"
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include "cavr/CavrEngine.h"
#include "dg/Lights.h"
#include "dg/Mesh.h"
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
  enableVR = true;
}

void cavr::CaveTestScene::Initialize() {
  dg::Scene::Initialize();

  // Create skybox.
  // Image generated from
  // http://wwwtyro.github.io/space-3d/#animationSpeed=0.9019276654012347&fov=76.5786603914366&nebulae=true&pointStars=true&resolution=2048&seed=2uj2ah7o9z00&stars=true&sun=true
  skybox = dg::Skybox::Create(
      dg::Texture::FromPath("assets/textures/stars_with_sun_skybox.png"));

  // Create sky light.
  skyLight = std::make_shared<dg::DirectionalLight>(
      glm::vec3(1.0f, 0.93f, 0.86f), 0.186f, 1.253f, 0.647f);
  skyLight->LookAtDirection(glm::vec3(0.183381f, -0.767736f, 0.613965f));
  dg::Behavior::Attach(skyLight,
                       std::make_shared<dg::KeyboardLightController>(window));
  AddChild(skyLight);

  // Create floor material.
  auto floorMaterial = std::make_shared<dg::StandardMaterial>(
      dg::StandardMaterial::WithTransparentColor(
          glm::vec4(glm::vec3(85, 43, 112) / 255.f, 0.4f)));
  floorMaterial->rasterizerOverride.SetCullMode(
      dg::RasterizerState::CullMode::OFF);

  // Create floor.
  const float floorThickness = 0.05f;
  AddChild(std::make_shared<dg::Model>(
      dg::Mesh::Cube, floorMaterial,
      dg::Transform::TS(glm::vec3(0, -floorThickness / 2, 0),
                        glm::vec3(2.34f, floorThickness, 1.8f))));

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

  // If VR could not enable, position the camera in a useful place for
  // development and make it controllable with keyboard and mouse.
  if (!enableVR) {
    window->LockCursor();
    mainCamera->transform = dg::Transform::T(glm::vec3(2.f, 1.83f, -1.2f));
    mainCamera->LookAtPoint(glm::vec3(0, 0, 0));
    dg::Behavior::Attach(
        mainCamera, std::make_shared<dg::KeyboardCameraController>(window));
  }

  // Create knot material for visualization.
  knotMaterial = std::make_shared<dg::StandardMaterial>(
      dg::StandardMaterial::WithTransparentColor(
          glm::vec4(0.5f, 0.1f, 0.1f, 0.4f)));
  knotMaterial->rasterizerOverride = dg::RasterizerState::AdditiveBlending();

  // Create knot vertex material for visualization.
  knotVertexMaterial = std::make_shared<dg::StandardMaterial>(
      dg::StandardMaterial::WithTransparentColor(
          glm::vec4(0.4f, 0.4f, 0.9f, 0.9f)));
  knotVertexMaterial->rasterizerOverride = dg::RasterizerState::AdditiveBlending();

  // Create segment material for visualization.
  segmentMaterial = std::make_shared<dg::StandardMaterial>(
      dg::StandardMaterial::WithTransparentColor(
          glm::vec4(0.1f, 0.5f, 0.1f, 0.3f)));
  segmentMaterial->rasterizerOverride = dg::RasterizerState::AdditiveBlending();
  segmentMaterial->rasterizerOverride.SetCullMode(
      dg::RasterizerState::CullMode::OFF);

  CreateCave();
}

void cavr::CaveTestScene::CreateCave() {
  // Cave container.
  auto cave = std::make_shared<dg::SceneObject>();
  AddChild(cave);

  // This tunnel's characteristics.
  const glm::vec3 segmentStart(-1.f, 1.2f, -0.3f);
  const glm::vec3 segmentEnd(1.f, 0.5f, 0.3f);
  const int numKnots = 5;
  const glm::vec3 segmentInterval =
      (segmentEnd - segmentStart) / (float)numKnots;
  const float minRadius = 0.15f;
  const float maxRadius = 0.25f;

  // Create the tunnel's knots.
  std::vector<std::shared_ptr<Tunnel::Knot>> knots;
  for (int i = 0; i < numKnots; i++) {
    float radius;
    if (i == 0) {
      radius = maxRadius;
    } else if (i == numKnots - 1) {
      radius = minRadius;
    } else {
      float pct = (float)(i - 1) / (float)(numKnots - 3);
      radius = maxRadius + (pct * (minRadius - maxRadius));
    }
    glm::vec3 pos = segmentStart + (segmentInterval * (float)i);
    glm::vec3 forward = glm::normalize(segmentInterval);
    knots.push_back(std::make_shared<Tunnel::Knot>(pos, forward, radius));
  }

  // Visualize each knot as a cylinder.
  for (auto &knot : knots) {
    cave->AddChild(CreateKnotModel(*knot), false);
  }

  // Create segments for each adjacent pair of knots.
  Tunnel tunnel;
  for (int i = 0; i < numKnots - 1; i++) {
    tunnel.segments.push_back(Tunnel::Segment(knots[i], knots[i + 1]));
  }

  // Visualize vertex positions for second segment.
  cave->AddChild(CreateKnotVertexModel(*tunnel.segments[1].start), false);
  cave->AddChild(CreateKnotVertexModel(*tunnel.segments[1].end), false);

  // Visualize second segment.
  tunnel.segments[1].CreateMesh();
}

std::shared_ptr<dg::SceneObject> cavr::CaveTestScene::CreateKnotVertexModel(
    const Tunnel::Knot &knot) const {
  auto container = std::make_shared<SceneObject>();
  for (int i = 0; i < Tunnel::VerticesPerRing; i++) {
    container->AddChild(
        std::make_shared<dg::Model>(
            dg::Mesh::Sphere, knotVertexMaterial,
            dg::Transform::TS(knot.GetVertexPosition(i), glm::vec3(0.01f))),
        false);
  }
  return container;
}

std::shared_ptr<dg::Model> cavr::CaveTestScene::CreateKnotModel(
    const Tunnel::Knot &knot) const {
  return std::make_shared<dg::Model>(
      dg::Mesh::Cylinder, knotMaterial,
      dg::Transform::TRS(
          knot.GetPosition(),
          glm::quat(knot.GetXF()) *
              glm::quat(glm::radians(glm::vec3(90, 0, 0))),
          glm::vec3(knot.GetRadius() * 2.f, 0.02f, knot.GetRadius() * 2.f)));
}

cavr::Tunnel::Knot::Knot(glm::vec3 position, glm::vec3 forward, float radius)
    : position(position), radius(radius) {
  glm::vec3 right = glm::normalize(glm::cross(forward, dg::UP));
  glm::vec3 up = glm::normalize(glm::cross(right, forward));
  xf = glm::mat3x3(right, up, -forward);
  for (int i = 0; i < Tunnel::VerticesPerRing; i++) {
    vertices[i] = PositionForVertex(i);
  }
}

glm::vec3 cavr::Tunnel::Knot::PositionForVertex(int index) const {
  return position + glm::quat(xf) *
                        glm::quat(glm::radians(
                            glm::vec3(0, 0, index * 360 / VerticesPerRing))) *
                        glm::vec3(radius, 0, 0);
}

cavr::Tunnel::Segment::Segment(std::shared_ptr<Knot> start,
                               std::shared_ptr<Knot> end)
    : start(start), end(end) {}

void cavr::Tunnel::Segment::CreateMesh() {
  for (int i = 0; i < VerticesPerRing; i++) {
    // TODO
  }
}

void cavr::CaveTestScene::Update() {
  Scene::Update();
}
