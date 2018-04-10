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
  enableVR = true;
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
  knotMaterial = std::make_shared<dg::StandardMaterial>(
      dg::StandardMaterial::WithTransparentColor(
          glm::vec4(0.5f, 0.1f, 0.1f, 0.4f)));

  // Create knot forward vector material for visualization.
  knotForwardMaterial = std::make_shared<dg::StandardMaterial>(
      dg::StandardMaterial::WithColor(glm::vec4(0.7f, 0.1f, 0.1f, 0.4f)));

  // Create knot vertex material for visualization.
  knotVertexMaterial = std::make_shared<dg::StandardMaterial>(
      dg::StandardMaterial::WithTransparentColor(
          glm::vec4(0.4f, 0.4f, 0.9f, 0.9f)));
  knotVertexMaterial->rasterizerOverride =
      dg::RasterizerState::AdditiveBlending();

  // Create segment material for visualization.
  glm::vec3 caveColor = glm::vec3(0.11f);
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
          glm::vec4(0.1f, 0.5f, 0.1f, 0.3f)));
  segmentWireframeMaterial->rasterizerOverride.SetCullMode(
      dg::RasterizerState::CullMode::FRONT);

  CreateCave();
}

std::vector<std::shared_ptr<cavr::Tunnel::Knot>>
cavr::CaveTestScene::CreateFunnelKnots() {
  const glm::vec3 segmentStart(1.f, 1.2f, 0.3f);
  const glm::vec3 segmentEnd(-1.f, 0.5f, -0.3f);
  const int numKnots = 35;
  const glm::vec3 segmentInterval =
      (segmentEnd - segmentStart) / (float)numKnots;
  const float minRadius = 0.1f;
  const float maxRadius = 0.25f;

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
    knots.push_back(std::make_shared<Tunnel::Knot>(pos, forward, radius, 1.f));
  }
  return knots;
}

std::vector<std::shared_ptr<cavr::Tunnel::Knot>>
cavr::CaveTestScene::CreateArcKnots() {
  std::vector<std::shared_ptr<Tunnel::Knot>> knots;

  const glm::vec3 start(1.f, 0.8f, 0.4f);
  const glm::vec3 middle(-0.1f, 1.3f, -0.2f);
  const glm::vec3 end(-1.f, 0.75f, 0.4f);
  const float radius = 0.13f;
  const float slope = -1.5;
  knots.push_back(std::shared_ptr<Tunnel::Knot>(new Tunnel::Knot(
      start, glm::normalize(glm::vec3(-0.1f, 0.4f, -0.5f)), radius, .3f)));
  knots.push_back(std::shared_ptr<Tunnel::Knot>(new Tunnel::Knot(
      middle, glm::normalize(glm::vec3(-1, 0, 0)), radius * 0.7f, 1.f)));
  knots.push_back(std::shared_ptr<Tunnel::Knot>(new Tunnel::Knot(
      end, glm::normalize(glm::vec3(-1.f, 0.f, 0.3f)), radius * 0.4f, 1.f)));

  return knots;
}

void cavr::CaveTestScene::CreateCave() {
  // Cave container.
  auto cave = std::make_shared<dg::SceneObject>(
      dg::Transform::R(glm::quat(glm::radians(glm::vec3(0, 180, 0)))));
  AddChild(cave);

  std::vector<std::shared_ptr<Tunnel::Knot>> knots = CreateArcKnots();

  // Visualize each original knots as models.
  this->knots = std::make_shared<dg::SceneObject>();
  cave->AddChild(this->knots, false);
  for (auto &knot : knots) {
    this->knots->AddChild(CreateKnotModels(*knot), false);
  }

  // Interpolate knots and finalize their vertices.
  knots = Tunnel::Knot::InterpolateKnots(knots);
  for (auto &knot : knots) {
    knot->CreateVertices();
  }

  // Create segments for each adjacent pair of knots.
  Tunnel tunnel;
  const size_t numKnots = knots.size();
  for (size_t i = 0; i < numKnots - 1; i++) {
    tunnel.AddSegment(Tunnel::Segment(knots[i], knots[i + 1]));
  }

  // Create mesh for tunnel.
  tunnel.CreateMesh();
  cave->AddChild(std::make_shared<dg::Model>(tunnel.GetMesh(), segmentMaterial,
                                             dg::Transform()),
                 false);
  auto outerMaterial =
      enableVR ? segmentTransparentMaterial : segmentWireframeMaterial;
  cave->AddChild(std::make_shared<dg::Model>(tunnel.GetMesh(), outerMaterial,
                                             dg::Transform()),
                 false);
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

std::shared_ptr<dg::SceneObject> cavr::CaveTestScene::CreateKnotModels(
    const Tunnel::Knot &knot) const {
  auto container = std::make_shared<SceneObject>();
  container->AddChild(
      std::make_shared<dg::Model>(
          dg::Mesh::Cylinder, knotMaterial,
          dg::Transform::TRS(knot.GetPosition(),
                             glm::quat(knot.GetXF()) *
                                 glm::quat(glm::radians(glm::vec3(-90, 0, 0))),
                             glm::vec3(knot.GetRadius() * 2.f, 0.012f,
                                       knot.GetRadius() * 2.f))),
      false);
  auto stem = std::make_shared<dg::Model>(
      dg::Mesh::Cylinder, knotForwardMaterial,
      dg::Transform::TRS(knot.GetPosition(),
                         glm::quat(knot.GetXF()) *
                             glm::quat(glm::radians(glm::vec3(-90, 1, 0))),
                         glm::vec3(0.005f, 0.05f, 0.005f)) *
          dg::Transform::T(glm::vec3(0.f, 0.5f, 0.f)));
  stem->AddChild(
      std::make_shared<dg::Model>(
          dg::Mesh::LoadOBJ("assets/models/cone.obj"), knotForwardMaterial,
          dg::Transform::TS(glm::vec3(0.f, 0.5f, 0.f),
                            glm::vec3(2.5f, 0.2f, 2.5f))),
      false);
  container->AddChild(stem, false);
  return container;
}

cavr::Tunnel::Knot::Knot(glm::vec3 position, glm::vec3 forward, float radius,
                         float curveSpeed)
    : position(position), radius(radius), curveSpeed(curveSpeed) {
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
    vertices[i] += (dir * r * radius * 0.1f);
  }
}

std::vector<std::shared_ptr<cavr::Tunnel::Knot>>
cavr::Tunnel::Knot::InterpolateKnots(
    const std::vector<std::shared_ptr<Knot>> &knots) {
  if (knots.size() <= 1) {
    return knots;
  }

  // TODO: Calculate based on length.
  const int subdivisions = 80;

  std::vector<std::shared_ptr<Knot>> newKnots;
  const size_t numKnots = knots.size();
  for (size_t i = 0; i < numKnots - 1; i++) {
    std::shared_ptr<Knot> first = knots[i];
    std::shared_ptr<Knot> second = knots[i + 1];

    newKnots.push_back(first);

    if (subdivisions <= 1) {
      continue;
    }

    // Use Cubic Hermite Curves to interpolate the positions.
    // http://www.cubic.org/docs/hermite.htm
    std::vector<glm::vec3> positions(subdivisions-1);
    for (int t = 1; t <= subdivisions - 1; t++) {
      const float s = (float)t / (float)subdivisions;
      const float h1 = (2 * s * s * s) - (3 * s * s) + 1;
      const float h2 = (-2 * s * s * s) + (3 * s * s);
      const float h3 = (s * s * s) - (2 * s * s) + s;
      const float h4 = (s * s * s) - (s * s);
      const float s1 = first->curveSpeed * sqrt(first->radius) * 10.f;
      const float s2 = second->curveSpeed * sqrt(second->radius) * 10.f;
      const glm::vec3 p1 = first->position;
      const glm::vec3 p2 = second->position;
      const glm::vec3 t1 = first->GetForward() * s1;
      const glm::vec3 t2 = second->GetForward() * s2;
      positions[t - 1] = (h1 * p1) + (h2 * p2) + (h3 * t1) + (h4 * t2);
    }

    for (int t = 1; t <= subdivisions - 1; t++) {
      // Interpolate the tangents based on next and previous knot positions.
      glm::vec3 pos = positions[t - 1];
      glm::vec3 prev = (t == 1 ? first->position : positions[t - 2]);
      glm::vec3 next =
          (t == (subdivisions - 1) ? second->position : positions[t]);
      glm::vec3 t1 = glm::normalize(next - pos);
      glm::vec3 t2 = glm::normalize(pos - prev);
      glm::vec3 forward = (t1 + t2) * 0.5f;

      // Smoothstep the radius and curveSpeed based on next and previous knots.
      float ss = glm::smoothstep(0.f, 1.f, (float)(t) / (float)subdivisions);
      float radius = first->radius + ((second->radius - first->radius) * ss);
      float curveSpeed =
          first->curveSpeed + ((second->curveSpeed - first->curveSpeed) * ss);

      newKnots.push_back(
          std::make_shared<Knot>(pos, forward, radius, curveSpeed));
    }
  }

  newKnots.push_back(knots.back());

  return newKnots;
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

    parity = 1 - parity;
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
