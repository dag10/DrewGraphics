//
//  scenes/RobotScene.cpp
//

#include <scenes/RobotScene.h>

#include <glm/glm.hpp>
#include <EngineTime.h>
#include <materials/UVMaterial.h>
#include <behaviors/KeyboardCameraController.h>

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

  // UV Material.
  auto robotMaterial = std::make_shared<UVMaterial>();

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
  if (enableVR) {
    float scale = 0.63f;
    robot->transform = Transform::TS(
      glm::vec3(0, (legLength + (torsoHeight / 2)) * scale, 0),
      glm::vec3(scale));
  }

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
      robotMaterial,
      Transform::TRS(
        { -legSeparation / 2, -torsoHeight / 2 - legLength / 2, 0 },
        glm::quat(glm::radians(glm::vec3(0, 180, 0))),
        { legDiameter, legLength, legDiameter }
        ));
  robot->AddChild(leftLeg, false);
  auto rightLeg = std::make_shared<Model>(
      Mesh::Cylinder,
      robotMaterial,
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
        robotMaterial,
        Transform::RS(
          glm::quat(glm::radians(glm::vec3(90, 0, 0))),
          { elbowDiameter, armDiameter * 1.1f, elbowDiameter }
          )), false);
  rightShoulder->AddChild(std::make_shared<Model>(
        Mesh::Cylinder,
        robotMaterial,
        Transform::RS(
          glm::quat(glm::radians(glm::vec3(90, 0, 0))),
          { elbowDiameter, armDiameter * 1.1f, elbowDiameter }
          )), false);

  // Upper arms.
  leftShoulder->AddChild(std::make_shared<Model>(
        Mesh::Cylinder,
        robotMaterial,
        Transform::TRS(
          { -upperArmLength / 2, 0, 0 },
          glm::quat(glm::radians(glm::vec3(0, 180, 90))),
          { armDiameter, upperArmLength, armDiameter }
          )), false);
  rightShoulder->AddChild(std::make_shared<Model>(
        Mesh::Cylinder,
        robotMaterial,
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
        robotMaterial,
        Transform::RS(
          glm::quat(glm::radians(glm::vec3(90, 0, 0))),
          { elbowDiameter, armDiameter * 1.1f, elbowDiameter }
          )), false);
  rightElbow->AddChild(std::make_shared<Model>(
        Mesh::Cylinder,
        robotMaterial,
        Transform::RS(
          glm::quat(glm::radians(glm::vec3(90, 0, 0))),
          { elbowDiameter, armDiameter * 1.1f, elbowDiameter }
          )), false);

  // Lower arms.
  leftElbow->AddChild(std::make_shared<Model>(
        Mesh::Cylinder,
        robotMaterial,
        Transform::TRS(
          { -lowerArmLength / 2, 0, 0 },
          glm::quat(glm::radians(glm::vec3(0, 180, 90))),
          { armDiameter, lowerArmLength, armDiameter }
          )), false);
  rightElbow->AddChild(std::make_shared<Model>(
        Mesh::Cylinder,
        robotMaterial,
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
        robotMaterial,
        Transform::RS(
          glm::quat(glm::radians(glm::vec3(0, 180, 0))),
          { neckDiameter, neckLength * 3, neckDiameter }
          )), false);

  // Head model.
  neck->AddChild(std::make_shared<Model>(
        Mesh::Cube,
        robotMaterial,
        Transform::TS(
          { 0, neckLength + (headSize / 2), 0 },
          { headSize, headSize, headSize}
          )), false);

  // Eyes container.
  eyes = std::make_shared<SceneObject>(
      Transform::T({ 0, neckLength + (headSize / 2) + eyeHeight, headSize / 2}));
  neck->AddChild(eyes, false);

  // Eyes
  eyes->AddChild(std::make_shared<Model>(
        Mesh::Cube,
        robotMaterial,
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
        robotMaterial,
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

  // If space is tapped, toggle between free-fly mode and wobble mode.
  if (window->IsKeyJustPressed(GLFW_KEY_SPACE)) {
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
      Time::Elapsed +
      ((float)rand()) / (float)RAND_MAX * maxBlinkInterval;
    endOfBlink = nextBlink + blinkDuration;
  }

  // Waving movement of left arm.
  float waveAmount = sin(Time::Elapsed * 13);
  float waveAmountOffset = sin(Time::Elapsed * 13 + 0.1f);

  leftShoulder->transform.rotation =
    glm::quat(glm::radians(glm::vec3(-15, 0, -10 + waveAmount * -2)));
  leftElbow->transform.rotation =
    glm::quat(glm::radians(glm::vec3(0, 0, -75 + waveAmountOffset * 20)));

  // Slow subtle movement of right arm.
  float rightArmMovement = sin(Time::Elapsed * 2);
  rightShoulder->transform.rotation =
    glm::quat(glm::radians(glm::vec3(0, 0, -70 + rightArmMovement * 2)));
  rightElbow->transform.rotation =
    glm::quat(glm::radians(glm::vec3(0, 0, -24 + rightArmMovement * -6)));

  // Slow wobble of camera.
  float cameraMovement = sin(Time::Elapsed * 1.0f + 0.7f);
  if (!freeFly && !enableVR) {
    mainCamera->transform =
      Transform::R(glm::quat(glm::radians(
              glm::vec3(0, cameraMovement * 20, 0)))) *
      Transform::T({ 0, 2, 4 });
    mainCamera->LookAt(*robot);
  }

  // Slow head wobble. Head also partially follows camera.
  float headMovement = cos(Time::Elapsed * 2);
  neck->transform.rotation =
    glm::quat(glm::radians(glm::vec3(-7, cameraMovement * 10, headMovement * 3)));
}

void dg::RobotScene::ClearBuffer() {
  glClearColor(0.4f, 0.6f, 0.75f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

