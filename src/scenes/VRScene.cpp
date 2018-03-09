//
//  scenes/VRScene.cpp
//

#include "dg/scenes/VRScene.h"

#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include "dg/Camera.h"
#include "dg/EngineTime.h"
#include "dg/Exceptions.h"
#include "dg/FrameBuffer.h"
#include "dg/Lights.h"
#include "dg/Mesh.h"
#include "dg/Model.h"
#include "dg/Shader.h"
#include "dg/Skybox.h"
#include "dg/Texture.h"
#include "dg/Transform.h"
#include "dg/Window.h"
#include "dg/behaviors/KeyboardLightController.h"
#include "dg/materials/StandardMaterial.h"
#include "dg/vr/VRManager.h"
#include "dg/vr/VRRenderModel.h"
#include "dg/vr/VRTrackedObject.h"

std::unique_ptr<dg::VRScene> dg::VRScene::Make() {
  return std::unique_ptr<dg::VRScene>(new dg::VRScene());
}

dg::VRScene::VRScene() : Scene() {
  enableVR = true;
}

void dg::VRScene::Initialize() {
  Scene::Initialize();

  std::cout
    << "This scene is a demo of OpenVR integration." << std::endl
    << std::endl
    << "Lighting controls:" << std::endl
    << "  I: Switch to indoor lighting (default)" << std::endl
    << "  F: Switch to camera-mounted flashlight" << std::endl
    << "  O: Switch to outdoor lighting" << std::endl
    << "  T: Switch to indoor spot lighting" << std::endl
    << "  L: Toggle ceiling light animation" << std::endl
    << std::endl
    << "Press ESC or Q to release the cursor, and press "
       "again to quit." << std::endl
    << std::endl;

  // Create textures.
  std::shared_ptr<Texture> crateTexture =
      Texture::FromPath("assets/textures/container2.png");
  std::shared_ptr<Texture> crateSpecularTexture =
      Texture::FromPath("assets/textures/container2_specular.png");
  std::shared_ptr<Texture> brickTexture =
      Texture::FromPath("assets/textures/brickwall.jpg");
  std::shared_ptr<Texture> brickNormalTexture =
      Texture::FromPath("assets/textures/brickwall_normal.jpg");
  std::shared_ptr<Texture> hardwoodTexture =
      Texture::FromPath("assets/textures/hardwood.jpg");
  std::shared_ptr<Texture> skyboxTexture =
      Texture::FromPath("assets/textures/skybox_daylight.png");

  // Create skybox.
  skybox = std::make_shared<Skybox>(skyboxTexture);
  skybox->material.SetInvPortal(glm::mat4x4(0));

  // Create sky light.
  skyLight = std::make_shared<DirectionalLight>(
      glm::vec3(1.0f, 0.93f, 0.86f),
      0.34f, 1.45f, 0.07f);
  skyLight->LookAtDirection(glm::normalize(glm::vec3(-0.3f, -1, -0.2f)));
  Behavior::Attach(skyLight, std::make_shared<KeyboardLightController>(window));
  AddChild(skyLight);

  // Create light cube material.
  StandardMaterial lightMaterial;
  lightMaterial.SetLit(false);

  // Create light cube.
  lightModel = std::make_shared<Model>(
      dg::Mesh::Cube,
      std::make_shared<StandardMaterial>(lightMaterial),
      Transform::TS(glm::vec3(1, 1.7f, 0), glm::vec3(0.05f)));
  AddChild(lightModel);

  // Create indoor and outdoor ceiling light source.
  glm::vec3 ceilingLightColor = glm::vec3(1.0f, 0.93f, 0.86f);
  spotLight = std::make_shared<SpotLight>(
      ceilingLightColor, 0.286f, 1.344f, 2.21f);
  spotLight->LookAtDirection(-UP);
  spotLight->SetCutoff(glm::radians(35.f));
  spotLight->SetFeather(glm::radians(3.f));
  indoorCeilingLight = std::make_shared<PointLight>(
      ceilingLightColor, 0.927f, 0.903f, 1.063f);
  outdoorCeilingLight = std::make_shared<PointLight>(
      ceilingLightColor, 0.134f, 0.518f, 0.803f);
  lightModel->AddChild(spotLight, false);
  lightModel->AddChild(indoorCeilingLight, false);
  lightModel->AddChild(outdoorCeilingLight, false);
  Behavior::Attach(
    spotLight, std::make_shared<KeyboardLightController>(window));
  Behavior::Attach(
    indoorCeilingLight, std::make_shared<KeyboardLightController>(window));
  Behavior::Attach(
    outdoorCeilingLight, std::make_shared<KeyboardLightController>(window));

  // Create wooden cube material.
  StandardMaterial cubeMaterial = StandardMaterial::WithTexture(crateTexture);
  cubeMaterial.SetSpecular(crateSpecularTexture);
  cubeMaterial.SetShininess(64);

  // Create wooden cubes.
  auto cubes = std::make_shared<SceneObject>();
  AddChild(cubes);
  const glm::vec3 cubePositions[] = {
    glm::vec3(0.0f,  0.25f,  0.0f),
    glm::vec3(-1.0f,  0.25f,  0.0f),
    glm::vec3(1.0f,  0.25f,  0.0f),
  };
  int numCubes = sizeof(cubePositions) / sizeof(cubePositions[0]);
  Model baseCubeModel(
      dg::Mesh::Cube,
      std::make_shared<StandardMaterial>(cubeMaterial),
      Transform::S(glm::vec3(0.5f)));
  for (int i = 0; i < numCubes; i++) {
    auto cube = std::make_shared<Model>(baseCubeModel);
    cube->transform.translation = cubePositions[i];
    cubes->AddChild(cube);
  }

  // Create wall material.
  StandardMaterial wallMaterial = StandardMaterial::WithTexture(brickTexture);
  wallMaterial.SetNormalMap(brickNormalTexture);
  wallMaterial.SetSpecular(0.2f);
  wallMaterial.SetShininess(64);

  // Create back wall.
  auto backWall = std::make_shared<Model>(
        dg::Mesh::Quad,
        std::make_shared<StandardMaterial>(wallMaterial),
        Transform::TRS(
          glm::vec3(1, 1, -1.5),
          glm::quat(glm::radians(glm::vec3(0))),
          glm::vec3(5, 2, 1)
          ));
  std::static_pointer_cast<StandardMaterial>(backWall->material)->SetUVScale(
      glm::vec2(5, 2));
  AddChild(backWall);

  // Create front wall, which is a copy of the back wall.
  auto frontWall = std::make_shared<Model>(*backWall);
  frontWall->transform = frontWall->transform * Transform::R(
      glm::quat(glm::radians(glm::vec3(0, 180, 0))));
  frontWall->transform.translation.z *= -1;
  AddChild(frontWall);

  // Create left wall.
  auto leftWall = std::make_shared<Model>(
        dg::Mesh::Quad,
        std::make_shared<StandardMaterial>(wallMaterial),
        Transform::TRS(
          glm::vec3(-1.5f, 1, 0),
          glm::quat(glm::radians(glm::vec3(0, 90, 0))),
          glm::vec3(3, 2, 1)
          ));
  std::static_pointer_cast<StandardMaterial>(leftWall->material)->SetUVScale(
      glm::vec2(3, 2));
  AddChild(leftWall);

  // Create right wall, which is a copy of the left wall.
  auto rightWall = std::make_shared<Model>(*leftWall);
  rightWall->transform = rightWall->transform * Transform::R(
      glm::quat(glm::radians(glm::vec3(0, 180, 0))));
  rightWall->transform.translation.x = 3.5f;
  AddChild(rightWall);

  // Create floor material.
  StandardMaterial floorMaterial =
      StandardMaterial::WithTexture(Texture::FromPath(
          "assets/textures/Flooring_Stone_001/Flooring_Stone_001_COLOR.png"));
  floorMaterial.SetNormalMap(Texture::FromPath(
      "assets/textures/Flooring_Stone_001/Flooring_Stone_001_NRM.png"));
  floorMaterial.SetSpecular(Texture::FromPath(
      "assets/textures/Flooring_Stone_001/Flooring_Stone_001_SPEC.png"));
  floorMaterial.SetShininess(9);
  floorMaterial.SetUVScale(glm::vec2(5, 3) * 2.f);
  floorMaterial.SetLit(true);

  // Create floor.
  auto floor = std::make_shared<Model>(
        dg::Mesh::Quad,
        std::make_shared<StandardMaterial>(floorMaterial),
        Transform::TRS(
          glm::vec3(1, 0, 0),
          glm::quat(glm::radians(glm::vec3(-90, 0, 0))),
          glm::vec3(5, 3, 1)
          ));
  AddChild(floor);

  // Create ceiling material.
  StandardMaterial ceilingMaterial = StandardMaterial::WithTexture(
      hardwoodTexture);
  ceilingMaterial.SetUVScale(glm::vec2(5, 3));
  ceilingMaterial.SetLit(true);
  ceilingMaterial.SetSpecular(0.1f);
  ceilingMaterial.SetShininess(32);

  // Create ceiling, which is a copy of the floor.
  ceiling= std::make_shared<Model>(*floor);
  ceiling->material = std::make_shared<StandardMaterial>(ceilingMaterial);
  ceiling->transform = ceiling->transform * Transform::R(
      glm::quat(glm::radians(glm::vec3(180, 0, 0))));
  ceiling->transform.translation.y = 2;
  AddChild(ceiling);

  // Create box that represents the camera's position.
  mainCamera->AddChild(std::make_shared<Model>(
      dg::Mesh::Cube,
      std::make_shared<Material>(
        StandardMaterial::WithColor(glm::vec3(0.8f, 1.0f, 0.8f))),
      Transform::S(glm::vec3(0.2f, 0.1f, 0.1f))), false);

  // Add objects to follow OpenVR tracked devices.
  auto leftController = std::make_shared<SceneObject>();
  vrContainer->AddChild(leftController);
  Behavior::Attach(leftController, std::make_shared<VRTrackedObject>(
    vr::ETrackedControllerRole::TrackedControllerRole_LeftHand));
  Behavior::Attach(leftController, std::make_shared<VRRenderModel>());
  auto rightController = std::make_shared<SceneObject>();
  Behavior::Attach(rightController, std::make_shared<VRTrackedObject>(
    vr::ETrackedControllerRole::TrackedControllerRole_RightHand));
  Behavior::Attach(rightController, std::make_shared<VRRenderModel>());
  vrContainer->AddChild(rightController);

  // Create controller block material.
  //StandardMaterial controllerMaterial =
  //    StandardMaterial::WithColor(glm::vec3(0.1f));
  //StandardMaterial controllerMaterial = StandardMaterial::WithTexture(
  //    VRManager::Instance->GetRenderModelTexture("vr_controller_vive_1_5"));
  //controllerMaterial.SetSpecular(0.3f);

  // Create blocks to represent left and right controllers.
  //auto leftControllerModel = std::make_shared<Model>(
  //  VRManager::Instance->GetRenderModelMesh("vr_controller_vive_1_5"),
  //  std::make_shared<StandardMaterial>(controllerMaterial),
  //  Transform());
  //leftController->AddChild(leftControllerModel, false);
  //auto rightControllerModel = std::make_shared<Model>(
  //  VRManager::Instance->GetRenderModelMesh("vr_controller_vive_1_5"),
  //  std::make_shared<StandardMaterial>(controllerMaterial),
  //  Transform());
  //rightController->AddChild(rightControllerModel, false);

  // Create a flashlight attached to the right controller.
  flashlight = std::make_shared<SpotLight>(
      ceilingLightColor, 0.314f, 2.16f, 2.11f);
  flashlight->LookAtDirection(FORWARD);
  flashlight->SetCutoff(glm::radians(35.f));
  Behavior::Attach(
    flashlight, std::make_shared<KeyboardLightController>(window));
  rightController->AddChild(flashlight, false);

  // Initial lighting configuration is the indoor point light.
  lightingType = PointLighting;
  UpdateLightingConfiguration();

  // Ceiling light is initially not moving.
  animatingLight = false;
}

void dg::VRScene::Update() {
  Scene::Update();

  // If F was tapped, switch to the flashlight configuration.
  if (window->IsKeyJustPressed(Key::F)) {
    lightingType = FlashlightLighting;
    UpdateLightingConfiguration();
  }

  // If T was tapped, switch to the spotlight configuration.
  if (window->IsKeyJustPressed(Key::T)) {
    lightingType = SpotLighting;
    UpdateLightingConfiguration();
  }

  // If I was tapped, switch to the indoor point light configuration.
  if (window->IsKeyJustPressed(Key::I)) {
    lightingType = PointLighting;
    UpdateLightingConfiguration();
  }

  // If O was tapped, switch to the outdoor configuration.
  if (window->IsKeyJustPressed(Key::O)) {
    lightingType = OutdoorLighting;
    UpdateLightingConfiguration();
  }

  // Toggle animating light with keyboard tap of L.
  if (window->IsKeyJustPressed(Key::L)) {
    animatingLight = !animatingLight;
  }

  // Animate light position.
  if (animatingLight) {
    lightModel->transform.translation.x = 1 + 1 * (float)sin(5 * Time::Elapsed);
  } else {
    lightModel->transform.translation.x = 1.5;
  }

  // Update light cube model to be consistent with point light.
  std::shared_ptr<Light> activeLight = nullptr;
  switch (lightingType) {
    case OutdoorLighting:
      activeLight = outdoorCeilingLight;
      break;
    case PointLighting:
      activeLight = indoorCeilingLight;
      break;
    case SpotLighting:
      activeLight = spotLight;
      break;
    case FlashlightLighting:
      activeLight = flashlight;
      break;
  }
  std::static_pointer_cast<StandardMaterial>(lightModel->material)
    ->SetDiffuse(activeLight->GetSpecular());
}

void dg::VRScene::UpdateLightingConfiguration() {
  skybox->enabled = (lightingType == OutdoorLighting);
  ceiling->enabled = (lightingType != OutdoorLighting);
  skyLight->enabled = (lightingType == OutdoorLighting);
  lightModel->enabled = (lightingType != FlashlightLighting);
  indoorCeilingLight->enabled = (lightingType == PointLighting);
  outdoorCeilingLight->enabled = (lightingType == OutdoorLighting);
  spotLight->enabled = (lightingType == SpotLighting);
  flashlight->enabled = (lightingType == FlashlightLighting);
}
