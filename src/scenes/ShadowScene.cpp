//
//  scenes/ShadowScene.cpp
//

#include "dg/scenes/ShadowScene.h"
#include <forward_list>
#include <glm/glm.hpp>
#include "dg/Camera.h"
#include "dg/EngineTime.h"
#include "dg/FrameBuffer.h"
#include "dg/Graphics.h"
#include "dg/Lights.h"
#include "dg/Mesh.h"
#include "dg/Model.h"
#include "dg/Shader.h"
#include "dg/Texture.h"
#include "dg/Window.h"
#include "dg/behaviors/KeyboardCameraController.h"
#include "dg/behaviors/KeyboardLightController.h"
#include "dg/materials/ScreenQuadMaterial.h"
#include "dg/materials/StandardMaterial.h"

std::unique_ptr<dg::ShadowScene> dg::ShadowScene::Make() {
  return std::unique_ptr<dg::ShadowScene>(new dg::ShadowScene());
}

dg::ShadowScene::ShadowScene() : Scene() {}

void dg::ShadowScene::Initialize() {
  Scene::Initialize();

  lightScene = std::make_shared<SceneObject>(Transform());
  AddChild(lightScene);

  // Lock window cursor to center.
  window->LockCursor();

  // Create textures.
  std::shared_ptr<Texture> crateTexture =
      Texture::FromPath("assets/textures/container2.png");
  std::shared_ptr<Texture> crateSpecularTexture =
      Texture::FromPath("assets/textures/container2_specular.png");
  std::shared_ptr<Texture> hardwoodTexture =
      Texture::FromPath("assets/textures/hardwood.jpg");

  // Create ceiling light source.
  spotlight = std::make_shared<SpotLight>(glm::vec3(1.0f, 0.93f, 0.86f), 0.31,
                                          0.91, 0.86);
  spotlight->transform.translation = glm::vec3(1.4f, 1.6f, -0.7f);
  spotlight->LookAtPoint({0, 0, 0});
  lightScene->AddChild(spotlight);

  // Make light controllable with keyboard.
  Behavior::Attach(spotlight,
                   std::make_shared<KeyboardLightController>(window));

  // Create light cone material.
  StandardMaterial lightMaterial =
      StandardMaterial::WithColor(spotlight->GetSpecular());
  lightMaterial.SetLit(false);

  // Create light cone.
  auto lightModel = std::make_shared<Model>(
      Mesh::LoadOBJ("assets/models/cone.obj"),
      std::make_shared<StandardMaterial>(lightMaterial),
      Transform::RS(glm::quat(glm::radians(glm::vec3(90, 0, 0))),
                    glm::vec3(0.05f)));
  spotlight->AddChild(lightModel, false);

  // Create frame buffer for light.
  //framebuffer = std::make_shared<FrameBuffer>(2048, 2048, true, false, false);
  framebuffer = std::make_shared<FrameBuffer>(2048, 2048, true, false, true);

  // Create wooden cube material.
  StandardMaterial cubeMaterial = StandardMaterial::WithTexture(crateTexture);
  cubeMaterial.SetSpecular(crateSpecularTexture);
  cubeMaterial.SetShininess(64);

  // Create wooden cube.
  cube = std::make_shared<Model>(
      dg::Mesh::Cube, std::make_shared<StandardMaterial>(cubeMaterial),
      Transform::TS(glm::vec3(0, 0.25f, 0), glm::vec3(0.5f)));
  lightScene->AddChild(cube);

  // Create floor material.
  const int floorSize = 500;
  StandardMaterial floorMaterial =
      StandardMaterial::WithTexture(hardwoodTexture);
  floorMaterial.SetUVScale(glm::vec2((float)floorSize));

  // Create floor plane.
  lightScene->AddChild(std::make_shared<Model>(
      dg::Mesh::Quad, std::make_shared<StandardMaterial>(floorMaterial),
      Transform::RS(glm::quat(glm::radians(glm::vec3(-90, 0, 0))),
                    glm::vec3(floorSize, floorSize, 1))));

  // Configure camera.
  mainCamera->transform = Transform::T({1.6, 1.74, 2.38});
  mainCamera->LookAtDirection({-0.279, -0.465, -0.841});

  // Allow camera to be controller by the keyboard and mouse.
  Behavior::Attach(mainCamera,
                   std::make_shared<KeyboardCameraController>(window));

  // Material for drawing depth map over scene.
  quadMaterial =
      std::make_shared<ScreenQuadMaterial>(glm::vec3(0), glm::vec2(1));

  CreateRobotScene();

  SetState(State::Spotlight);
}

void dg::ShadowScene::CreateRobotScene() {
  robotScene = std::make_shared<SceneObject>(Transform::S(glm::vec3(0.6f)));
  AddChild(robotScene);

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
  auto robot = std::make_shared<SceneObject>(
      Transform::TR({0, legLength + (torsoHeight * 0.5f), 0},
                    glm::quat(glm::radians(glm::vec3(0, 45, 0)))));
  robotScene->AddChild(robot);

  // Create light container.
  lightContainer = std::make_shared<SceneObject>();
  robotScene->AddChild(lightContainer);

  // Create light model materials.
  StandardMaterial lightModelMaterial = StandardMaterial::WithColor(
    { 0.8f, 0.5f, 0.5f });

  // Create point light.
  auto pointLight = std::make_shared<PointLight>(
    glm::vec3(1.f, 0.23f, 0.36f), 0.227f, 1.33f, 2.063f);
  pointLight->transform.translation = { 0, 0.2f, -2 };
  //lightContainer->AddChild(pointLight, false);

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
  robotScene->AddChild(std::make_shared<Model>(
      Mesh::Cube, cubeMaterial,
      Transform::TRS({-2.0, cubeSize * 0.5f, 2.1},
                     glm::quat(glm::radians(glm::vec3(0, 40, 0))),
                     glm::vec3(cubeSize))));
  robotScene->AddChild(std::make_shared<Model>(
      Mesh::Cube, cubeMaterial,
      Transform::TRS({-2.2, cubeSize * 1.5f, 2.2},
                     glm::quat(glm::radians(glm::vec3(0, 10, 0))),
                     glm::vec3(cubeSize))));
  robotScene->AddChild(std::make_shared<Model>(
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
  robotScene->AddChild(roomSides);

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
  glm::vec2 wallSize(floorSize, 10);
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
                     Mesh::Cube, eyeMaterial,
                     Transform::TS({-eyeSeparation / 2 - eyeSize / 2, 0,
                                    faceFeatureDepth / 2},
                                   {eyeSize, eyeSize, faceFeatureDepth})),
                 false);
  eyes->AddChild(std::make_shared<Model>(
                     Mesh::Cube, eyeMaterial,
                     Transform::TS({eyeSeparation / 2 + eyeSize / 2, 0,
                                    faceFeatureDepth / 2},
                                   {eyeSize, eyeSize, faceFeatureDepth})),
                 false);

  // Create a spotlight.
  robotSpotlight = std::make_shared<SpotLight>(
    glm::vec3(0.4f, 0.63f, 0.86f), 1.444f, 3.76f, 3.11f);
  robotSpotlight->SetCutoff(glm::radians(30.f));
  robotSpotlight->SetFeather(glm::radians(2.f));
  robotScene->AddChild(robotSpotlight, false);
  robotSpotlight->SetSceneSpace(Transform::T({2.186,0.401,5.74}));
  robotSpotlight->LookAtDirection({-0.42, 0.116, -0.9});

  // Create light cone material.
  StandardMaterial lightMaterial =
      StandardMaterial::WithColor(robotSpotlight->GetSpecular());
  lightMaterial.SetLit(false);

  // Create light cone.
  auto lightModel = std::make_shared<Model>(
      Mesh::LoadOBJ("assets/models/cone.obj"),
      std::make_shared<StandardMaterial>(lightMaterial),
      Transform::RS(glm::quat(glm::radians(glm::vec3(90, 0, 0))),
                    glm::vec3(0.2f)));
  robotSpotlight->AddChild(lightModel, false);
}

void dg::ShadowScene::Update() {
  Scene::Update();

  // Slowly rotate light.
  spotlight->transform =
      Transform::R(glm::quat(glm::radians(glm::vec3(0, Time::Delta * 30, 0)))) *
      spotlight->transform;

  // Check for demo state change.
  if (window->IsKeyJustPressed(Key::SPACE)) {
    SetState((State)((int)state+1));
  } else if (window->IsKeyJustPressed(Key::NUM_1)) {
    SetState(State::Spotlight);
  } else if (window->IsKeyJustPressed(Key::NUM_2)) {
    SetState(State::RenderLight);
  } else if (window->IsKeyJustPressed(Key::NUM_3)) {
    SetState(State::RenderDepth);
  } else if (window->IsKeyJustPressed(Key::NUM_4)) {
    SetState(State::Shadow);
  } else if (window->IsKeyJustPressed(Key::NUM_5)) {
    SetState(State::Feather);
  } else if (window->IsKeyJustPressed(Key::NUM_6)) {
    SetState(State::Robot);
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

void dg::ShadowScene::SetState(State state) {
  this->state = state;
  spotlight->SetFeather((int)state >= (int)State::Feather ? glm::radians(5.f)
                                                          : 0.f);
  robotScene->enabled = ((int)state >= (int)State::Robot);
  robotSpotlight->enabled = robotScene->enabled;
  lightScene->enabled = !robotScene->enabled;
  if (state == State::Spotlight) {
    mainCamera->transform = Transform::T({1.6, 1.74, 2.38});
    mainCamera->LookAtDirection({-0.279, -0.465, -0.841});
  } else if (state == State::Robot) {
    mainCamera->transform =
        Transform::TR(glm::vec3(7.7922, 2.7799, 5.9404),
                      glm::quat(glm::vec3(-0.1167, 0.7811, 0)));
  }
}

void dg::ShadowScene::RenderFrame() {
#if defined(_OPENGL)
  auto& light = robotScene->enabled ? robotSpotlight : spotlight;

  // Render scene for light framebuffer.
  Camera lightCamera;
  lightCamera.transform = light->SceneSpace();
  lightCamera.fov = light->GetCutoff() * 2;
  lightCamera.nearClip = 0.5;
  lightCamera.farClip = robotScene->enabled ? 15 : 7;
  lightTransform =
      lightCamera.GetProjectionMatrix() * lightCamera.GetViewMatrix();
  framebuffer->Bind();
  framebuffer->SetViewport();

  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  DrawScene(lightCamera);

  framebuffer->Unbind();
  window->ResetViewport();

#endif

  ClearBuffer();
  ConfigureBuffer();

  // Render scene for real.
  mainCamera->aspectRatio = window->GetAspectRatio();
  DrawScene(*mainCamera);


  if ((int)state >= (int)State::RenderLight) {
    // Draw light color rendering over scene.
    glm::vec2 scale = glm::vec2(1) / glm::vec2(window->GetAspectRatio(), 1);
    quadMaterial->SetTexture(framebuffer->GetColorTexture());
    quadMaterial->SetRedChannelOnly(false);
    quadMaterial->SetScale(scale);
    quadMaterial->SetOffset(glm::vec2(1 - scale.x * 0.5, scale.y * 0.5));
    quadMaterial->Use();
    Mesh::Quad->Draw();
  }

  if ((int)state >= (int)State::RenderDepth) {
    // Draw depth map over scene.
    glm::vec2 scale = glm::vec2(1) / glm::vec2(window->GetAspectRatio(), 1);
    quadMaterial->SetTexture(framebuffer->GetDepthTexture());
    quadMaterial->SetScale(scale);
    quadMaterial->SetRedChannelOnly(true);
    quadMaterial->SetOffset(glm::vec2(1 - scale.x * 0.5, -1 + scale.y * 0.5));
    quadMaterial->Use();
    Mesh::Quad->Draw();
  }
}

void dg::ShadowScene::PrepareModelForDraw(
    const Model& model,
    glm::vec3 cameraPosition,
    glm::mat4x4 view,
    glm::mat4x4 projection,
    const Light::ShaderData(&lights)[Light::MAX_LIGHTS]) const {
  Scene::PrepareModelForDraw(model, cameraPosition, view, projection, lights);
  if ((int)state >= (int)State::Shadow) {
    model.material->SendShadowMap(framebuffer->GetDepthTexture());
    model.material->SendLightTransform(lightTransform);
  } else {
    model.material->SendLightTransform(glm::mat4(0));
  }
}
