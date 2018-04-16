//
//  behaviors/CaveBehavior.cpp
//

#include "cavr/behaviors/CaveBehavior.h"
#include "dg/EngineTime.h"
#include "dg/Model.h"
#include "dg/SceneObject.h"
#include "dg/materials/StandardMaterial.h"

void cavr::CaveBehavior::Initialize() {
	dg::Behavior::Initialize();
}

void cavr::CaveBehavior::Start() {
	dg::Behavior::Start();
  auto obj = GetSceneObject();

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
  knots = std::make_shared<dg::SceneObject>();
  obj->AddChild(knots, false);
  caveInteriorModels = std::make_shared<dg::SceneObject>();
  obj->AddChild(caveInteriorModels, false);
  caveWireframeModels = std::make_shared<dg::SceneObject>();
  obj->AddChild(caveWireframeModels, false);
  caveTransparentModels = std::make_shared<dg::SceneObject>();
  obj->AddChild(caveTransparentModels, false);
  caveTransparentModels->enabled = true;
  caveWireframeModels->enabled = !caveTransparentModels->enabled;

  // Create initial knot sets.
  knotSets.push_back(CreateArcKnots().WithInterpolatedKnots());
  knotSets.push_back(CreateStraightKnots().WithInterpolatedKnots());

  // Update initial properties.
  SetShowKnots(GetShowKnots());
  SetShowWireframe(GetShowWireframe());

  // Add initial cave segments.
  for (int i = 0; i < 30; i++) {
    AddNextCaveSegment();
  }
}

void cavr::CaveBehavior::Update() {
	dg::Behavior::Update();
  GetSceneObject()->transform.translation += velocity * (float)dg::Time::Delta;
}

void cavr::CaveBehavior::SetVelocity(glm::vec3 velocity) {
  this->velocity = velocity;
}

void cavr::CaveBehavior::AddVelocity(glm::vec3 velocity) {
  this->velocity += velocity;
}

glm::vec3 cavr::CaveBehavior::GetVelocity() const {
  return velocity;
}

void cavr::CaveBehavior::SetShowKnots(bool showKnots) {
  this->showKnots = showKnots;
  if (knots != nullptr) {
    knots->enabled = showKnots;
  }
}

bool cavr::CaveBehavior::GetShowKnots() const {
  return showKnots;
}

void cavr::CaveBehavior::SetShowWireframe(bool showWireframe) {
  this->showWireframe = showWireframe;
  if (caveWireframeModels != nullptr) {
    caveWireframeModels->enabled = showWireframe;
  }
  if (caveTransparentModels != nullptr) {
    caveTransparentModels->enabled = !showWireframe;
  }
}

bool cavr::CaveBehavior::GetShowWireframe() const {
  return showWireframe;
}

void cavr::CaveBehavior::AddNextCaveSegment() {
  const CaveSegment::KnotSet &knotSet = knotSets[rand() % knotSets.size()];
  AddNextCaveSegment(knotSet);
}

void cavr::CaveBehavior::AddNextCaveSegment(
    const CaveSegment::KnotSet &knotSet) {
  const CaveSegment::KnotSet interpKnots =
      knotSet.IsInterpolated() ? knotSet : knotSet.WithInterpolatedKnots();
  std::unique_ptr<CaveSegment> nextCaveSegment;
  if (lastCaveSegment == nullptr) {
    nextCaveSegment =
        std::unique_ptr<CaveSegment>(new CaveSegment(interpKnots));
  } else {
    nextCaveSegment = std::unique_ptr<CaveSegment>(
        new CaveSegment(interpKnots, *lastCaveSegment));
  }
  AddCaveSegment(*nextCaveSegment);
  lastCaveSegment = std::move(nextCaveSegment);
}

cavr::CaveSegment::KnotSet cavr::CaveBehavior::CreateArcKnots() {
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
      end, glm::normalize(glm::vec3(-1.f, 0.f, 0.3f)), radius * 0.95f, 15.f)));

  return set;
}

cavr::CaveSegment::KnotSet cavr::CaveBehavior::CreateStraightKnots() {
  CaveSegment::KnotSet set;

  const glm::vec3 start(1.f, 0.75f, 0.f);
  const glm::vec3 end(-1.f, 0.75f, 0.f);
  const float radius = 0.15f;
  set.knots.push_back(std::shared_ptr<CaveSegment::Knot>(
      new CaveSegment::Knot(start, -dg::RIGHT, radius, 15.f)));
  set.knots.push_back(std::shared_ptr<CaveSegment::Knot>(
      new CaveSegment::Knot(end, -dg::RIGHT, radius * 0.95f, 15.f)));

  return set;
}

void cavr::CaveBehavior::AddCaveSegment(const CaveSegment &segment) {
  // Visualize each original knots as models.
  for (auto &knot : segment.GetKnotSet().IsInterpolated()
                        ? segment.GetKnotSet().noninterpolatedKnots
                        : segment.GetKnotSet().knots) {
    knots->AddChild(CreateKnotVisualization(*knot), false);
  }

  // Create mesh for tunnel.
  caveInteriorModels->AddChild(
      std::make_shared<dg::Model>(segment.GetMesh(), caveMaterial,
                                  dg::Transform()),
      false);
  caveWireframeModels->AddChild(
      std::make_shared<dg::Model>(segment.GetMesh(), caveWireframeMaterial,
                                  dg::Transform()),
      false);
  caveTransparentModels->AddChild(
      std::make_shared<dg::Model>(segment.GetMesh(), caveTransparentMaterial,
                                  dg::Transform()),
      false);
}

std::shared_ptr<dg::SceneObject>
cavr::CaveBehavior::CreateKnotVertexVisualization(
    const CaveSegment::Knot &knot) const {
  auto container = std::make_shared<dg::SceneObject>();
  for (int i = 0; i < CaveSegment::VerticesPerRing; i++) {
    container->AddChild(
        std::make_shared<dg::Model>(
            dg::Mesh::Sphere, ringVertexMaterial,
            dg::Transform::TS(knot.GetVertexPosition(i), glm::vec3(0.002f))),
        false);
  }
  return container;
}

std::shared_ptr<dg::SceneObject> cavr::CaveBehavior::CreateKnotVisualization(
    const CaveSegment::Knot &knot) const {
  auto container = std::make_shared<dg::SceneObject>();

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

  // "Up" cone
  auto up = std::make_shared<dg::Model>(
      dg::Mesh::LoadOBJ("assets/models/cone.obj"), knotArrowMaterial,
      knot.GetXF() *
          dg::Transform::TS(dg::UP * 1.0f, glm::vec3(diskWidth * 2.f)));
  container->AddChild(up, false);

  return container;
}
