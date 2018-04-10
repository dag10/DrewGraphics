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

  // Create skybox.
  // Image generated from
  // http://wwwtyro.github.io/space-3d/#animationSpeed=0.9019276654012347&fov=76.5786603914366&nebulae=true&pointStars=true&resolution=2048&seed=2uj2ah7o9z00&stars=true&sun=true
  skybox = dg::Skybox::Create(
      dg::Texture::FromPath("assets/textures/stars_with_sun_skybox.png"));

  // Create sky light.
  auto skyLight = std::make_shared<dg::DirectionalLight>(
      glm::vec3(1.0f, 0.93f, 0.86f), 0.f, 1.1f, 0.647f);
  skyLight->LookAtDirection(glm::vec3(0.183381f, -0.767736f, 0.613965f));
  dg::Behavior::Attach(skyLight,
                       std::make_shared<dg::KeyboardLightController>(window));
  AddChild(skyLight);

  // Create moon light to shade shadow from sun.
  auto moonLight = std::make_shared<dg::DirectionalLight>(
      glm::vec3(1.0f, 0.93f, 0.86f), 0.f, 0.35f, 0.f);
  moonLight->LookAtDirection(-skyLight->transform.Forward());
  AddChild(moonLight);

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

  // Attach point light to left controller.
  controllerLight = std::make_shared<dg::PointLight>(
      glm::vec3(1.0f, 0.93f, 0.86f), 0.f, 5.0f, 3.f);
  controllerLight->transform = dg::Transform::T(dg::FORWARD * 0.035f);
  controllerLight->LookAtDirection(-skyLight->transform.Forward());
  controllerLight->SetLinear(1.5f);
  controllerLight->SetQuadratic(3.0f);
  leftController->AddChild(controllerLight, false);

  // Attach sphere to left controller to represent point light.
  auto lightSphereColor = std::make_shared<dg::StandardMaterial>(
      dg::StandardMaterial::WithColor(controllerLight->GetDiffuse()));
  lightSphereColor->SetLit(false);
  controllerLight->AddChild(
      std::make_shared<dg::Model>(dg::Mesh::Sphere, lightSphereColor,
                                  dg::Transform::S(glm::vec3(0.025f))),
      false);

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
    controllerLight->transform =
        dg::Transform::T((dg::FORWARD * 0.2f) + (dg::UP * -0.08f));
    mainCamera->AddChild(controllerLight, false);
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
  knotVertexMaterial->rasterizerOverride =
      dg::RasterizerState::AdditiveBlending();

  // Create segment material for visualization.
  glm::vec3 caveColor = glm::vec3(112, 103, 87) / 255.f * 0.4f;
  segmentMaterial = std::make_shared<dg::StandardMaterial>(
      dg::StandardMaterial::WithColor(caveColor));

  // Create transparent wireframe material for visualization.
  segmentTransparentMaterial = std::make_shared<dg::StandardMaterial>(
      dg::StandardMaterial::WithTransparentColor(glm::vec4(caveColor, 0.3f)));
  segmentTransparentMaterial->rasterizerOverride.SetCullMode(
      dg::RasterizerState::CullMode::FRONT);

  // Create segment wireframe material for visualization.
  segmentWireframeMaterial = std::make_shared<dg::StandardMaterial>(
      dg::StandardMaterial::WithWireframeColor(
          glm::vec4(0.1f, 0.5f, 0.1f, 0.1f)));
  segmentWireframeMaterial->rasterizerOverride.SetCullMode(
      dg::RasterizerState::CullMode::FRONT);

  CreateCave();
}

void cavr::CaveTestScene::CreateCave() {
  // Cave container.
  auto cave = std::make_shared<dg::SceneObject>();
  AddChild(cave);

  // This tunnel's characteristics.
  const glm::vec3 segmentStart(-1.f, 1.2f, -0.3f);
  const glm::vec3 segmentEnd(1.f, 0.5f, 0.3f);
  const int numKnots = 35;
  const glm::vec3 segmentInterval =
      (segmentEnd - segmentStart) / (float)numKnots;
  const float minRadius = 0.1f;
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

  // Visualize each original knot as a cylinder.
  //for (auto &knot : knots) {
  //  cave->AddChild(CreateKnotModel(*knot), false);
  //}

  // Interpolate knots and finalize their vertices.
  knots = Tunnel::Knot::InterpolateKnots(knots);
  for (auto &knot : knots) {
    knot->CreateVertices();
  }

  // Create segments for each adjacent pair of knots.
  Tunnel tunnel;
  for (int i = 0; i < numKnots - 1; i++) {
    tunnel.AddSegment(Tunnel::Segment(knots[i], knots[i + 1]));
  }

  // Create mesh for tunnel.
  tunnel.CreateMesh();
  cave->AddChild(std::make_shared<dg::Model>(tunnel.GetMesh(), segmentMaterial,
                                             dg::Transform()));
  cave->AddChild(std::make_shared<dg::Model>(
      tunnel.GetMesh(), segmentTransparentMaterial, dg::Transform()));
}

std::shared_ptr<dg::SceneObject> cavr::CaveTestScene::CreateKnotVertexModels(
    const Tunnel::Knot &knot) const {
  auto container = std::make_shared<SceneObject>();
  for (int i = 0; i < Tunnel::VerticesPerRing; i++) {
    container->AddChild(
        std::make_shared<dg::Model>(
            dg::Mesh::Sphere, knotVertexMaterial,
            dg::Transform::TS(knot.GetVertexPosition(i), glm::vec3(0.002f))),
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
          glm::vec3(knot.GetRadius() * 2.f, 0.012f, knot.GetRadius() * 2.f)));
}

cavr::Tunnel::Knot::Knot(glm::vec3 position, glm::vec3 forward, float radius)
    : position(position), radius(radius) {
  glm::vec3 right = glm::normalize(glm::cross(forward, dg::UP));
  glm::vec3 up = glm::normalize(glm::cross(right, forward));
  xf = glm::mat3x3(right, up, -forward);
}

void cavr::Tunnel::Knot::CreateVertices() {
  assert(vertices.empty());
  vertices = std::vector<glm::vec3>(VerticesPerRing);
  for (int i = 0; i < Tunnel::VerticesPerRing; i++) {
    vertices[i] = position + glm::quat(xf) *
                                 glm::quat(glm::radians(glm::vec3(
                                     0, 0, i * 360 / VerticesPerRing))) *
                                 glm::vec3(radius, 0, 0);

    // Randomize the position a little bit to make it bumpy.
    float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    glm::vec3 dir = glm::normalize(vertices[i] - position);
    vertices[i] += (dir * r * 0.02f);
  }
}

std::vector<std::shared_ptr<cavr::Tunnel::Knot>>
cavr::Tunnel::Knot::InterpolateKnots(
    const std::vector<std::shared_ptr<Knot>> &knots) {
  return knots; // TODO
}

cavr::Tunnel::Segment::Segment(std::shared_ptr<Knot> start,
                               std::shared_ptr<Knot> end)
    : knots{start, end} {}

void cavr::Tunnel::Segment::CreateMesh(
    std::vector<dg::Mesh::Triangle> &triangles, int parity) {
  const int numTriangles = VerticesPerRing * 2;

  for (int i = 0; i < VerticesPerRing; i++) {
    int nextIdx = (i + 1) % VerticesPerRing;
    int a = parity;
    int b = 1 - a;
    dg::Vertex v1(knots[a]->GetVertexPosition(i));
    dg::Vertex v2(knots[b]->GetVertexPosition(i));
    dg::Vertex v3(knots[b]->GetVertexPosition(nextIdx));
    dg::Vertex v4(knots[a]->GetVertexPosition(nextIdx));

    auto winding =
        (parity == 1) ? dg::Mesh::Winding::CW : dg::Mesh::Winding::CCW;

    triangles.push_back(dg::Mesh::Triangle(v1, v2, v3, winding));
    triangles.push_back(dg::Mesh::Triangle(v1, v3, v4, winding));
  }
}

void cavr::Tunnel::AddSegment(const Segment &segment) {
  assert(mesh == nullptr);
  segments.push_back(segment);
}

void cavr::Tunnel::CreateMesh() {
  std::vector<dg::Mesh::Triangle> triangles;
  int parity = 0;
  for (auto &segment : segments) {
    segment.CreateMesh(triangles, parity);
    parity = 1 - parity;
  }
  mesh = dg::Mesh::Create();
  for (auto &triangle : triangles) {
    triangle.CalculateFaceNormal();
    mesh->AddTriangle(triangle);
  }
  mesh->FinishBuilding();
}

void cavr::CaveTestScene::Update() {
  Scene::Update();

  if (window->IsKeyJustPressed(dg::Key::SPACE)) {
    controllerLight->enabled = !controllerLight->enabled;
  }
}
