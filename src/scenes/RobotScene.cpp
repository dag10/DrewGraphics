//
//  scenes/RobotScene.cpp
//

#include <scenes/RobotScene.h>

#include <EngineTime.h>
#include <Graphics.h>
#include <Lights.h>
#include <Mesh.h>
#include <Model.h>
#include <behaviors/KeyboardCameraController.h>
#include <materials/StandardMaterial.h>
#include <materials/UVMaterial.h>
#include <glm/glm.hpp>

std::unique_ptr<dg::RobotScene> dg::RobotScene::Make() {
  return std::unique_ptr<dg::RobotScene>(new dg::RobotScene(false));
}

std::unique_ptr<dg::RobotScene> dg::RobotScene::MakeVR() {
  return std::unique_ptr<dg::RobotScene>(new dg::RobotScene(true));
}

dg::RobotScene::RobotScene(bool enableVR) : Scene() {
  this->enableVR = enableVR;
}

void dg::RobotScene::Initialize() {
  Scene::Initialize();

  // Lock window cursor to center.
  if (!enableVR) {
    window->LockCursor();
  }

  // Create sky light.
  auto skylight = std::make_shared<DirectionalLight>(
    glm::vec3(1.0f, 0.93f, 0.86f),
    0.14f, 0.35f, 0.17f);
  skylight->LookAtDirection({ -0.3f, -1.f, -0.2f });
  AddChild(skylight);

  // Robot parameters.
  float torsoHeight = 1;
  float torsoWidth = 0.7f;
  float legDiameter = 0.25f;
  float legSeparation = 0.4f;
  float legLength = 0.9f;
  float shoulderHeight = torsoHeight / 4;
  float armDiameter = 0.2f;
  float elbowDiameter = 0.21f;
  float upperArmLength = 0.40f;
  float lowerArmLength = 0.50f;
  float neckDiameter = 0.15f;
  float neckLength = 0.1f;
  float headSize = 0.45f;
  float faceFeatureDepth = 0.05f;
  float eyeHeight = headSize * 0.3f;
  float eyeSize = 0.1f;
  float eyeSeparation = 0.1f;

  // Robot container.
  robot = std::make_shared<SceneObject>();
  AddChild(robot);

  // Create light container.
  lightContainer = std::make_shared<SceneObject>();
  AddChild(lightContainer);

  // Create light model materials.
  StandardMaterial lightModelMaterial = StandardMaterial::WithColor(
    { 0.8f, 0.5f, 0.5f });

  // Create point light.
  auto pointLight = std::make_shared<PointLight>(
    glm::vec3(1.f, 0.23f, 0.36f), 0.227f, 1.33f, 2.063f);
  pointLight->transform.translation = { 0, 1.0f, -2 };
  lightContainer->AddChild(pointLight, false);

  // Add point light sphere.
  lightModelMaterial.SetDiffuse(pointLight->GetDiffuse());
  pointLight->AddChild(std::make_shared<Model>(
    Mesh::Sphere,
    std::make_shared<StandardMaterial>(lightModelMaterial),
    Transform::S({ 0.2f, 0.2f, 0.2f })), false);

  // Create spot light.
  auto spotLight = std::make_shared<SpotLight>(
    glm::vec3(0.4f, 0.63f, 0.86f), 0.186f, 1.344f, 2.21f);
  spotLight->SetFeather(glm::radians(3.f));
  spotLight->transform = Transform::TR(
      { 0, 2, 2 },
      glm::quat(glm::radians(glm::vec3(-50, 0, 0))));
  lightContainer->AddChild(spotLight, false);

  // Add spot light cone.
  lightModelMaterial.SetDiffuse(spotLight->GetDiffuse());
  spotLight->AddChild(std::make_shared<Model>(
    Mesh::LoadOBJ("assets/models/cone.obj"),
    std::make_shared<StandardMaterial>(lightModelMaterial),
    Transform::RS(
      glm::quat(glm::radians(glm::vec3(90, 0, 0))),
      { 0.2f, 0.2f, 0.2f }
  )), false);

  // Robot materials.
  std::shared_ptr<StandardMaterial> robotMaterial =
    std::make_shared<StandardMaterial>();
  robotMaterial->SetDiffuse({ 0.1f, 0.3f, 0.1f });
  robotMaterial->SetSpecular({ 0.3f, 0.4f, 0.3f });

  // Head material.
  auto headMaterial = std::make_shared<StandardMaterial>(*robotMaterial);
  headMaterial->SetDiffuse({ 0.1f, 0.25f, 0.17f });

  // Eye material.
  auto eyeMaterial = std::make_shared<StandardMaterial>(*robotMaterial);
  eyeMaterial->SetDiffuse({ 0.1f, 0.13f, 0.1f });

  // Joint material.
  auto jointMaterial = std::make_shared<StandardMaterial>(*robotMaterial);
  jointMaterial->SetDiffuse({ 0.07f, 0.27f, 0.15f });

  // Leg material.
  auto legMaterial = std::make_shared<StandardMaterial>(*robotMaterial);
  legMaterial->SetDiffuse({ 0.1f, 0.32f, 0.2f });

  // Arm material.
  auto armMaterial = std::make_shared<StandardMaterial>(*robotMaterial);
  armMaterial->SetDiffuse({ 0.15f, 0.35f, 0.2f });

  // Floor material.
  StandardMaterial floorMaterial = StandardMaterial::WithColor(
    glm::vec3(0.4f, 0.6f, 0.75f));

  // Add floor quad.
  const float floorSize = 2000;
  AddChild(std::make_shared<Model>(
    Mesh::Quad,
    std::make_shared<StandardMaterial>(floorMaterial),
    Transform::TRS(
      { 0, -legLength - (torsoHeight * 0.5f), 0 },
      glm::quat(glm::radians(glm::vec3(-90, 0, 0))),
      { floorSize, floorSize, floorSize }
  )));

  // OBJ model material.
  auto objMaterial = std::make_shared<StandardMaterial>(
    StandardMaterial::WithColor(glm::vec3(0.3f)));
  objMaterial->SetSpecular(glm::vec3(0.9f));

  // Load torus.
  AddChild(std::make_shared<Model>(
    Mesh::LoadOBJ("assets/models/torus.obj"),
    objMaterial,
    Transform::TS(
      { -1.5f, 0, 0 },
      { 0.8f, 0.8f, 0.8f }
  )));

  // Load helix.
  AddChild(std::make_shared<Model>(
    Mesh::LoadOBJ("assets/models/helix.obj"),
    objMaterial,
    Transform::TS(
      { 1.2f, 0, 0 },
      { 0.5f, 0.5f, 0.5f }
  )));

  // Torso
  auto torso = std::make_shared<Model>(
    Mesh::Cube,
    robotMaterial,
    Transform::TS(
      { 0, 0, 0 },
      { torsoWidth, torsoHeight, 0.4f }
  ));
  robot->AddChild(torso, false);

  // Legs
  auto leftLeg = std::make_shared<Model>(
    Mesh::Cylinder,
    legMaterial,
    Transform::TRS(
      { -legSeparation / 2, -torsoHeight / 2 - legLength / 2, 0 },
      glm::quat(glm::radians(glm::vec3(0, 180, 0))),
      { legDiameter, legLength, legDiameter }
  ));
  robot->AddChild(leftLeg, false);
  auto rightLeg = std::make_shared<Model>(
    Mesh::Cylinder,
    legMaterial,
    Transform::TRS(
      { legSeparation / 2, -torsoHeight / 2 - legLength / 2, 0 },
      glm::quat(glm::radians(glm::vec3(0, 180, 0))),
      { legDiameter, legLength, legDiameter }
  ));
  robot->AddChild(rightLeg, false);

  // Shoulder joints.
  leftShoulder = std::make_shared<SceneObject>(
    Transform::T({
      -torsoWidth / 2 - elbowDiameter * 0.2f,
      shoulderHeight,
      0 }));
  robot->AddChild(leftShoulder, false);
  rightShoulder = std::make_shared<SceneObject>(
    Transform::T({
      torsoWidth / 2 + elbowDiameter * 0.2f,
      shoulderHeight,
      0 }));
  robot->AddChild(rightShoulder, false);

  // Shoulder models.
  leftShoulder->AddChild(std::make_shared<Model>(
    Mesh::Cylinder,
    jointMaterial,
    Transform::RS(
      glm::quat(glm::radians(glm::vec3(90, 0, 0))),
      { elbowDiameter, armDiameter * 1.1f, elbowDiameter }
  )), false);
  rightShoulder->AddChild(std::make_shared<Model>(
    Mesh::Cylinder,
    jointMaterial,
    Transform::RS(
      glm::quat(glm::radians(glm::vec3(90, 0, 0))),
      { elbowDiameter, armDiameter * 1.1f, elbowDiameter }
  )), false);

  // Upper arms.
  leftShoulder->AddChild(std::make_shared<Model>(
    Mesh::Cylinder,
    armMaterial,
    Transform::TRS(
      { -upperArmLength / 2, 0, 0 },
      glm::quat(glm::radians(glm::vec3(0, 180, 90))),
      { armDiameter, upperArmLength, armDiameter }
  )), false);
  rightShoulder->AddChild(std::make_shared<Model>(
    Mesh::Cylinder,
    armMaterial,
    Transform::TRS(
      { upperArmLength / 2, 0, 0 },
      glm::quat(glm::radians(glm::vec3(0, 180, 90))),
      { armDiameter, upperArmLength, armDiameter }
  )), false);

  // Elbow joints.
  leftElbow = std::make_shared<SceneObject>(
    Transform::T({ -upperArmLength, 0, 0 }));
  leftShoulder->AddChild(leftElbow, false);
  rightElbow = std::make_shared<SceneObject>(
    Transform::T({ upperArmLength, 0, 0 }));
  rightShoulder->AddChild(rightElbow, false);

  // Elbow models.
  leftElbow->AddChild(std::make_shared<Model>(
    Mesh::Cylinder,
    jointMaterial,
    Transform::RS(
      glm::quat(glm::radians(glm::vec3(90, 0, 0))),
      { elbowDiameter, armDiameter * 1.1f, elbowDiameter }
  )), false);
  rightElbow->AddChild(std::make_shared<Model>(
    Mesh::Cylinder,
    jointMaterial,
    Transform::RS(
      glm::quat(glm::radians(glm::vec3(90, 0, 0))),
      { elbowDiameter, armDiameter * 1.1f, elbowDiameter }
  )), false);

  // Lower arms.
  leftElbow->AddChild(std::make_shared<Model>(
    Mesh::Cylinder,
    armMaterial,
    Transform::TRS(
      { -lowerArmLength / 2, 0, 0 },
      glm::quat(glm::radians(glm::vec3(0, 180, 90))),
      { armDiameter, lowerArmLength, armDiameter }
  )), false);
  rightElbow->AddChild(std::make_shared<Model>(
    Mesh::Cylinder,
    armMaterial,
    Transform::TRS(
      { lowerArmLength / 2, 0, 0 },
      glm::quat(glm::radians(glm::vec3(0, 180, 90))),
      { armDiameter, lowerArmLength, armDiameter }
  )), false);

  // Neck joint.
  neck = std::make_shared<SceneObject>(
    Transform::T({ 0, torsoHeight / 2, 0 }));
  robot->AddChild(neck, false);

  // Neck model.
  neck->AddChild(std::make_shared<Model>(
    Mesh::Cylinder,
    headMaterial,
    Transform::RS(
      glm::quat(glm::radians(glm::vec3(0, 180, 0))),
      { neckDiameter, neckLength * 3, neckDiameter })
    ), false);

  // Head model.
  neck->AddChild(std::make_shared<Model>(
    Mesh::Cube,
    headMaterial,
    Transform::TS(
      { 0, neckLength + (headSize / 2), 0 },
      { headSize, headSize, headSize }
  )), false);

  // Eyes container.
  eyes = std::make_shared<SceneObject>(
    Transform::T({ 0, neckLength + (headSize / 2) + eyeHeight, headSize / 2 }));
  neck->AddChild(eyes, false);

  // Eyes
  eyes->AddChild(std::make_shared<Model>(
    Mesh::Cube,
    eyeMaterial,
    Transform::TS(
      {
        -eyeSeparation / 2 - eyeSize / 2,
        0,
        faceFeatureDepth / 2
      },
      { eyeSize, eyeSize, faceFeatureDepth }
  )), false);
  eyes->AddChild(std::make_shared<Model>(
    Mesh::Cube,
    eyeMaterial,
    Transform::TS(
      {
        eyeSeparation / 2 + eyeSize / 2,
        0,
        faceFeatureDepth / 2
      },
      { eyeSize, eyeSize, faceFeatureDepth }
  )), false);


  // Allow camera to be controller by the keyboard and mouse.
  if (!enableVR) {
    Behavior::Attach(
        mainCamera,
        std::make_shared<KeyboardCameraController>(window));
  }
}

void dg::RobotScene::Update() {
  Scene::Update();

	// Make lights orbit scene.
  lightContainer->transform.rotation =
    glm::quat(glm::radians(glm::vec3(0, Time::Elapsed * 50, 0)));

  // If space is tapped, toggle between free-fly mode and wobble mode.
  if (window->IsKeyJustPressed(Key::SPACE)) {
    freeFly = !freeFly;
  }

  // Blink at random intervals.
  if (Time::Elapsed >= nextBlink && Time::Elapsed < endOfBlink) {
    eyes->transform.scale.y = 0.1f;
  } else if (Time::Elapsed > endOfBlink) {
    eyes->transform.scale.y = 1;
    const float blinkDuration = 0.1f; // seconds
    const float maxBlinkInterval = 3.0f; // seconds
    nextBlink =
      (float)Time::Elapsed +
      ((float)rand()) / (float)RAND_MAX * maxBlinkInterval;
    endOfBlink = nextBlink + blinkDuration;
  }

  // Waving movement of left arm.
  float waveAmount = (float)sin(Time::Elapsed * 13);
  float waveAmountOffset = (float)sin(Time::Elapsed * 13 + 0.1f);

  leftShoulder->transform.rotation =
    glm::quat(glm::radians(glm::vec3(-15, 0, -10 + waveAmount * -2)));
  leftElbow->transform.rotation =
    glm::quat(glm::radians(glm::vec3(0, 0, -75 + waveAmountOffset * 20)));

  // Slow subtle movement of right arm.
  float rightArmMovement = (float)sin(Time::Elapsed * 2);
  rightShoulder->transform.rotation =
    glm::quat(glm::radians(glm::vec3(0, 0, -70 + rightArmMovement * 2)));
  rightElbow->transform.rotation =
    glm::quat(glm::radians(glm::vec3(0, 0, -24 + rightArmMovement * -6)));

  // Slow wobble of camera.
  float cameraMovement = (float)sin(Time::Elapsed * 0.7f + 0.7f);
  if (!freeFly && !enableVR) {
    mainCamera->transform =
      Transform::R(glm::quat(glm::radians(
              glm::vec3(0, cameraMovement * 20, 0)))) *
      Transform::T({ 0, 2.5, 4 });
    mainCamera->LookAt(*robot);
  }

  // Slow head wobble. Head also partially follows camera.
  float headMovement = (float)cos(Time::Elapsed * 2);
  neck->transform.rotation =
    glm::quat(glm::radians(glm::vec3(-11, cameraMovement * 10, headMovement * 3)));
}

void dg::RobotScene::ClearBuffer() {
  Graphics::Instance->Clear(glm::vec3(26.f, 37.f, 43.f) / 255.f);
}

