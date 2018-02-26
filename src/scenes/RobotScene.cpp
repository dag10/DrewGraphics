//
//  scenes/RobotScene.cpp
//

#include "dg/scenes/RobotScene.h"

#include <glm/glm.hpp>
#include <iostream>
#include "dg/EngineTime.h"
#include "dg/Graphics.h"
#include "dg/Lights.h"
#include "dg/Mesh.h"
#include "dg/Model.h"
#include "dg/behaviors/KeyboardCameraController.h"
#include "dg/behaviors/KeyboardLightController.h"
#include "dg/materials/StandardMaterial.h"

std::unique_ptr<dg::RobotScene> dg::RobotScene::Make() {
  return std::unique_ptr<dg::RobotScene>(new dg::RobotScene());
}

void dg::RobotScene::Initialize() {
  Scene::Initialize();

  std::cout << "This scene is a demo of lighting, textures, specularity maps, "
            << "and normal maps." << std::endl
            << std::endl
            << "Light controls:" << std::endl
            << "  F:       Toggle flashlight" << std::endl
            << std::endl
            << "Camera controls:" << std::endl
            << "  Mouse:   Look around" << std::endl
            << "  W:       Move forward" << std::endl
            << "  A:       Move left" << std::endl
            << "  S:       Move backward" << std::endl
            << "  D:       Move right" << std::endl
            << "  Shift:   Increase movement speed" << std::endl
            << "  Control: Decrease movement speed" << std::endl
            << "  R:       Reset camera to initial pose" << std::endl
            << "  C:       Print current camera pose" << std::endl
            << std::endl
            << "Press ESC or Q to release the cursor, and press "
               "again to quit."
            << std::endl
            << std::endl;

  // Lock window cursor to center.
  window->LockCursor();

  // Create sky light.
  auto skylight = std::make_shared<DirectionalLight>(
    glm::vec3(1.0f, 0.93f, 0.86f),
    0.017f, 0.086f, 0.17f);
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
  robot = std::make_shared<SceneObject>(
      Transform::TR({0, legLength + (torsoHeight * 0.5f), 0},
                    glm::quat(glm::radians(glm::vec3(0, 45, 0)))));
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
  pointLight->transform.translation = { 0, 0.2f, -2 };
  lightContainer->AddChild(pointLight, false);

  // Add point light sphere.
  pointLight->AddChild(
      std::make_shared<Model>(
          Mesh::Sphere,
          std::make_shared<StandardMaterial>(
              StandardMaterial::WithColor(pointLight->GetSpecular())),
          Transform::S({0.2f, 0.2f, 0.2f})),
      false);

  // Create wooden cube material.
  auto cubeMaterial =
      std::make_shared<StandardMaterial>(StandardMaterial::WithTexture(
          Texture::FromPath("assets/textures/container2.png")));
  cubeMaterial->SetSpecular(
      Texture::FromPath("assets/textures/container2_specular.png"));
  cubeMaterial->SetShininess(64);

  // Create wooden cubes.
  float cubeSize = 0.9f;
  AddChild(std::make_shared<Model>(
      Mesh::Cube, cubeMaterial,
      Transform::TRS({-2.0, cubeSize * 0.5f, 2.1},
                     glm::quat(glm::radians(glm::vec3(0, 40, 0))),
                     glm::vec3(cubeSize))));
  AddChild(std::make_shared<Model>(
      Mesh::Cube, cubeMaterial,
      Transform::TRS({-2.2, cubeSize * 1.5f, 2.2},
                     glm::quat(glm::radians(glm::vec3(0, 10, 0))),
                     glm::vec3(cubeSize))));
  AddChild(std::make_shared<Model>(
      Mesh::Cube, cubeMaterial,
      Transform::TRS({2.1, cubeSize * 0.5f, -1.8},
                     glm::quat(glm::radians(glm::vec3(0, -34, 0))),
                     glm::vec3(cubeSize))));

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

  // Room wall and floor container.
  auto roomSides = std::make_shared<SceneObject>(Transform::T({ -3, 0, -3 }));
  AddChild(roomSides);

  // Floor material.
  const int floorSize = 500;
  StandardMaterial floorMaterial =
      StandardMaterial::WithTexture(Texture::FromPath(
          "assets/textures/Flooring_Stone_001/Flooring_Stone_001_COLOR.png"));
  floorMaterial.SetNormalMap(Texture::FromPath(
      "assets/textures/Flooring_Stone_001/Flooring_Stone_001_NRM.png"));
  floorMaterial.SetSpecular(Texture::FromPath(
      "assets/textures/Flooring_Stone_001/Flooring_Stone_001_SPEC.png"));
  floorMaterial.SetShininess(9);
  floorMaterial.SetUVScale(glm::vec2((float)floorSize));
  floorMaterial.SetLit(true);

  // Add floor quad.
  roomSides->AddChild(std::make_shared<Model>(
    Mesh::Quad,
    std::make_shared<StandardMaterial>(floorMaterial),
    Transform::TRS(
      { floorSize / 2, 0, floorSize / 2 },
      glm::quat(glm::radians(glm::vec3(-90, 0, 0))),
      { floorSize, floorSize, 1 }
  )), false);

  // Wall material.
  glm::vec2 wallSize(floorSize, 6);
  StandardMaterial wallMaterial = StandardMaterial::WithTexture(
      Texture::FromPath("assets/textures/brickwall.jpg"));
  wallMaterial.SetNormalMap(
      Texture::FromPath("assets/textures/brickwall_normal.jpg"));
  wallMaterial.SetSpecular(0.2f);
  wallMaterial.SetUVScale(wallSize / 2.f);
  wallMaterial.SetShininess(64);

  // Create walls.
  roomSides->AddChild(std::make_shared<Model>(
    Mesh::Quad,
    std::make_shared<StandardMaterial>(wallMaterial),
    Transform::TRS(
      { wallSize.x / 2, wallSize.y / 2, 0 },
      glm::quat(glm::radians(glm::vec3(0, 0, 0))),
      { wallSize.x, wallSize.y, 1 }
  )), false);
  roomSides->AddChild(std::make_shared<Model>(
    Mesh::Quad,
    std::make_shared<StandardMaterial>(wallMaterial),
    Transform::TRS(
      { 0, wallSize.y / 2, wallSize.x / 2 },
      glm::quat(glm::radians(glm::vec3(0, 90, 0))),
      { wallSize.x, wallSize.y, 1 }
  )), false);

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

  // Initial camera position.
  mainCamera->transform = Transform::T({ 2.f, 2.36f, 3.64f });
  mainCamera->LookAtDirection({ -0.524f, -0.324f, -0.788f });

  // Allow camera to be controller by the keyboard and mouse.
  Behavior::Attach(
      mainCamera,
      std::make_shared<KeyboardCameraController>(window, 7.f));

  // Create a flashlight attached to the camera.
  flashlight = std::make_shared<SpotLight>(
    glm::vec3(0.4f, 0.63f, 0.86f), 0.244f, 2.76f, 3.11f);
  flashlight->SetCutoff(glm::radians(25.f));
  flashlight->SetFeather(glm::radians(2.f));
  flashlight->SetCutoff(glm::radians(25.f));
  flashlight->transform = Transform::T(glm::vec3(0.1f, -0.1f, 0));
  Behavior::Attach(
    flashlight, std::make_shared<KeyboardLightController>(window));
  mainCamera->AddChild(flashlight, false);
}

void dg::RobotScene::Update() {
  Scene::Update();

  // If F is tapped, toggle flashlight.
  if (window->IsKeyJustPressed(Key::F)) {
    flashlight->enabled = !flashlight->enabled;
  }

	// Make lights orbit scene.
  lightContainer->transform.rotation =
    glm::quat(glm::radians(glm::vec3(0, Time::Elapsed * 50, 0)));

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

  // Slow head wobble.
  float sideMovement = (float)sin(Time::Elapsed * 0.7f + 0.7f);
  float headMovement = (float)cos(Time::Elapsed * 2);
  neck->transform.rotation =
    glm::quat(glm::radians(glm::vec3(-11, sideMovement * 10, headMovement * 3)));
}

void dg::RobotScene::ClearBuffer() {
  Graphics::Instance->Clear(glm::vec3(26.f, 37.f, 43.f) / 255.f);
}

