//
//  behaviors/CaveBehavior.cpp
//

#include "cavr/behaviors/CaveBehavior.h"
#include "cavr/scenes/GameScene.h"
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

  // Cave shader and material.
  // TODO: Refactor
  auto cavePurple = glm::vec3(165, 16, 239) / 255.f * 0.117f * 2.f;
  auto baseCaveMaterial = std::make_shared<dg::StandardMaterial>(
      dg::StandardMaterial::WithTransparentColor(glm::vec4(cavePurple, 0.8)));
  baseCaveMaterial->shader =
      dg::Shader::FromFiles("CaveVertexShader.cso", "CavePixelShader.cso");

  // Create inner cave material.
  glm::vec3 caveColor = glm::vec3(0.11f);
  //caveMaterial = std::make_shared<dg::StandardMaterial>(
  //    dg::StandardMaterial::WithColor(caveColor));
  caveMaterial = std::shared_ptr<dg::StandardMaterial>(
      new dg::StandardMaterial(*baseCaveMaterial));
  caveMaterial->SetProperty("isFront", false);

  // Create outer transparent cave material.
  //caveTransparentMaterial = std::make_shared<dg::StandardMaterial>(
  //    dg::StandardMaterial::WithTransparentColor(glm::vec4(caveColor, 0.3f)));
  caveTransparentMaterial =
      std::make_shared<dg::StandardMaterial>(*baseCaveMaterial);
  caveTransparentMaterial->SetDiffuse(glm::vec4(cavePurple, 0.5f));
  caveTransparentMaterial->SetProperty("isFront", true);
  caveTransparentMaterial->queue = caveMaterial->queue + 1;
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
  // TODO: Add actual probability weights, not just repetition in the set.
  float constriction = 0.993f;
  for (int i = 0; i < 10; i++) {
    knotSets.push_back(CreateArcKnots(constriction).WithInterpolatedKnots());
  }
  for (int angle = -20; angle <= 20; angle += 10) {
    for (int rot = -20; rot <= 20; rot += 10) {
      knotSets.push_back(
          CreateSimpleCurve(rot, angle, constriction).WithInterpolatedKnots());
    }
  }
  for (int rot = -20; rot <= 20; rot += 4) {
    knotSets.push_back(
        CreateSimpleCurve(rot, 5, constriction).WithInterpolatedKnots());
  }
  for (int i = 0; i < 6; i++) {
    knotSets.push_back(CreateStraightKnots(constriction).WithInterpolatedKnots());
  }

  // Update initial properties.
  SetShowKnots(GetShowKnots());
  SetShowWireframe(GetShowWireframe());

  // Add initial cave segments.
  AddNextCaveSegment(CreateCaveStart(0.15f));
  for (int i = 0; i < 2; i++) {
    AddNextCaveSegment(CreateStraightKnots(1));
  }
  for (int i = 0; i < 200; i++) {
    AddNextCaveSegment();
  }
}

void cavr::CaveBehavior::Update() {
	dg::Behavior::Update();
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

void cavr::CaveBehavior::SetCrashPosition(glm::vec3 pos) {
  caveMaterial->SetProperty("crashPosition", pos);
  caveTransparentMaterial->SetProperty("crashPosition", pos);
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

// Rotation: [0, 90)
// Angle: [0, 180)
cavr::CaveSegment::KnotSet cavr::CaveBehavior::CreateSimpleCurve(
    float rotation, float angle, float constriction) {
  CaveSegment::KnotSet set;

  glm::quat rot(glm::radians(glm::vec3(0, angle * 0.5f, rotation)));
  float speed = 25.0f + 15.0f * angle / 180.f;

  const glm::vec3 start(1.f, 0.75f, 0.f);
  const glm::vec3 end(-1.f, 0.75f, 0.f);
  const float radius = 0.1f;
  set.knots.push_back(std::shared_ptr<CaveSegment::Knot>(
      new CaveSegment::Knot(start, rot * -dg::RIGHT, radius, speed)));
  set.knots.push_back(std::shared_ptr<CaveSegment::Knot>(new CaveSegment::Knot(
      end, glm::inverse(rot) * -dg::RIGHT, radius * constriction, speed)));

  return set;
}

cavr::CaveSegment::KnotSet cavr::CaveBehavior::CreateArcKnots(
    float constriction) {
  CaveSegment::KnotSet set;

  float halfConstriction = (constriction + 1.f) * 0.5f;

  const glm::vec3 start(1.f, 0.8f, 0.4f);
  const glm::vec3 middle(-0.1f, 1.3f, -0.2f);
  const glm::vec3 end(-1.f, 0.75f, 0.4f);
  const float radius = 0.15f;
  set.knots.push_back(std::shared_ptr<CaveSegment::Knot>(new CaveSegment::Knot(
      start, glm::normalize(glm::vec3(-0.1f, 0.4f, -0.5f)), radius, 15.f)));
  set.knots.push_back(std::shared_ptr<CaveSegment::Knot>(
      new CaveSegment::Knot(middle, glm::normalize(glm::vec3(-1, 0, 0)),
                            radius * halfConstriction, 15.f)));
  set.knots.push_back(std::shared_ptr<CaveSegment::Knot>(
      new CaveSegment::Knot(end, glm::normalize(glm::vec3(-1.f, 0.f, 0.3f)),
                            radius * constriction, 15.f)));

  return set;
}

cavr::CaveSegment::KnotSet cavr::CaveBehavior::CreateCaveStart(float radius) {
  CaveSegment::KnotSet set;
  set.bumpy = false;

  const glm::vec3 start(-0.5f, 0.f, 0.f);
  const glm::vec3 end = start + glm::vec3(0.1f, 0.f, 0.f);
  set.knots.push_back(std::shared_ptr<CaveSegment::Knot>(
      new CaveSegment::Knot(start, dg::RIGHT, 0, 1)));
  set.knots.push_back(std::shared_ptr<CaveSegment::Knot>(
      new CaveSegment::Knot(end, dg::RIGHT, radius, 1)));

  return set;
}

cavr::CaveSegment::KnotSet cavr::CaveBehavior::CreateStraightKnots(
    float constriction) {
  CaveSegment::KnotSet set;

  const glm::vec3 start(1.f, 0.75f, 0.f);
  const glm::vec3 end(-1.f, 0.75f, 0.f);
  const float radius = 0.15f;
  set.knots.push_back(std::shared_ptr<CaveSegment::Knot>(
      new CaveSegment::Knot(start, -dg::RIGHT, radius, 15.f)));
  set.knots.push_back(std::shared_ptr<CaveSegment::Knot>(
      new CaveSegment::Knot(end, -dg::RIGHT, radius * constriction, 15.f)));

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
  auto interior = std::make_shared<dg::Model>(segment.GetMesh(), caveMaterial,
                                              dg::Transform());
  interior->layer = GameScene::LayerMask::CaveGeometry();
  caveInteriorModels->AddChild(interior, false);
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
