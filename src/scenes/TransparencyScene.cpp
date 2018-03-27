//
//  scenes/TransparencyScene.h
//

#include "dg/scenes/TransparencyScene.h"

#include <forward_list>
#include <glm/glm.hpp>
#include <iostream>
#include "dg/Camera.h"
#include "dg/EngineTime.h"
#include "dg/Lights.h"
#include "dg/Mesh.h"
#include "dg/Model.h"
#include "dg/Shader.h"
#include "dg/Skybox.h"
#include "dg/Texture.h"
#include "dg/Window.h"
#include "dg/behaviors/KeyboardCameraController.h"
#include "dg/materials/StandardMaterial.h"

std::unique_ptr<dg::TransparencyScene> dg::TransparencyScene::Make() {
  return std::unique_ptr<TransparencyScene>(new TransparencyScene(false));
}

std::unique_ptr<dg::TransparencyScene> dg::TransparencyScene::MakeVR() {
  return std::unique_ptr<TransparencyScene>(new TransparencyScene(true));
}

dg::TransparencyScene::TransparencyScene(bool enableVR) : Scene() {
  this->enableVR = enableVR;
}

void dg::TransparencyScene::Initialize() {
  Scene::Initialize();

  std::cout
    << "This scene is a demo of transparent material rendering." << std::endl
    << std::endl;
  if (!enableVR) {
    std::cout
      << "Camera controls:" << std::endl
      << "  Mouse: Look around" << std::endl
      << "  W: Move forward" << std::endl
      << "  A: Move left" << std::endl
      << "  S: Move backward" << std::endl
      << "  D: Move right" << std::endl
      << "  Shift: Increase move speed" << std::endl
      << "  R: Reset camera to initial position" << std::endl
      << "  C: Print current camera pose" << std::endl
      << std::endl
      << "Press ESC or Q to release the cursor, and press "
         "again to quit." << std::endl
      << std::endl;
  }

  // Lock window cursor to center.
  if (!enableVR) {
    window->LockCursor();
  }

  // Create textures.
  std::shared_ptr<Texture> hardwoodTexture =
      Texture::FromPath("assets/textures/hardwood.jpg");
  std::shared_ptr<Texture> skyboxTexture =
      Texture::FromPath("assets/textures/skybox_daylight.png");

  // Create skybox.
  skybox = Skybox::Create(skyboxTexture);

  // Create ceiling light source.
  auto ceilingLight = std::make_shared<PointLight>(
      glm::vec3(1, 0.93, 0.86), 0.732f, 0.399f, 0.968f);
  ceilingLight->transform.translation = glm::vec3(0, 0.8, -0.5);
  AddChild(ceilingLight);

  // Container for additively blended meshes.
  auto additiveMeshes =
      std::make_shared<SceneObject>(Transform::T(FORWARD * -1.f));
  AddChild(additiveMeshes);

  // Additive material.
  StandardMaterial additiveMaterial =
      StandardMaterial::WithTransparentColor(glm::vec4(1, 1, 1, 0.5f));
  //additiveMaterial.rasterizerOverride = RasterizerState::AlphaBlending();
  //additiveMaterial.queue = RenderQueue::Transparent;
  float additiveAlpha = 0.5f;

  // Create additive cube.
  additiveMaterial.SetDiffuse(
      glm::vec4(glm::vec3(75, 0, 130) / 255.f, additiveAlpha));
  additiveMeshes->AddChild(std::make_shared<Model>(
      Mesh::Cube,
      std::make_shared<StandardMaterial>(additiveMaterial),
      Transform::TS(glm::vec3(-2, 0.25, 0), glm::vec3(0.5))), false);

  // Create additive mapped cube.
  additiveMaterial.SetDiffuse(
      glm::vec4(glm::vec3(0, 0, 255) / 255.f, additiveAlpha));
  additiveMeshes->AddChild(std::make_shared<Model>(
      Mesh::MappedCube,
      std::make_shared<StandardMaterial>(additiveMaterial),
      Transform::TS(glm::vec3(-1, 0.25, 0), glm::vec3(0.5))), false);

  // Create additive quad.
  additiveMaterial.SetDiffuse(
      glm::vec4(glm::vec3(0, 255, 0) / 255.f, additiveAlpha));
  additiveMaterial.rasterizerOverride.SetCullMode(
      RasterizerState::CullMode::OFF);
  additiveMeshes->AddChild(std::make_shared<Model>(
      Mesh::Quad,
      std::make_shared<StandardMaterial>(additiveMaterial),
      Transform::TS(glm::vec3(0, 0.25, 0), glm::vec3(0.5))), false);
  additiveMaterial.rasterizerOverride.ClearCullMode();

  // Create additive sphere.
  additiveMaterial.SetDiffuse(
      glm::vec4(glm::vec3(255, 255, 0) / 255.f, additiveAlpha));
  additiveMeshes->AddChild(std::make_shared<Model>(
      Mesh::Sphere,
      std::make_shared<StandardMaterial>(additiveMaterial),
      Transform::TS(glm::vec3(1, 0.25, 0), glm::vec3(0.5))), false);

  // Create additive cylinder.
  additiveMaterial.SetDiffuse(
      glm::vec4(glm::vec3(255, 127, 0) / 255.f, additiveAlpha));
  additiveMeshes->AddChild(std::make_shared<Model>(
      Mesh::Cylinder,
      std::make_shared<StandardMaterial>(additiveMaterial),
      Transform::TS(glm::vec3(2, 0.25, 0), glm::vec3(0.5))), false);

  // Create shiny brick material.
  StandardMaterial brickMaterial = StandardMaterial::WithTexture(
      Texture::FromPath("assets/textures/brickwall.jpg"));
  brickMaterial.SetNormalMap(
      Texture::FromPath("assets/textures/brickwall_normal.jpg"));
  brickMaterial.SetSpecular(0.6f);
  brickMaterial.SetShininess(64);

  // Container for textured meshes.
  auto texturedTransparency = std::make_shared<SceneObject>(
      Transform::T(FORWARD * 1.f));
  AddChild(texturedTransparency);

  // Create textured cube.
  texturedTransparency->AddChild(std::make_shared<Model>(
      Mesh::Cube,
      std::make_shared<StandardMaterial>(brickMaterial),
      Transform::TS(glm::vec3(-2, 0.25, 0), glm::vec3(0.5))), false);

  // Create textured mapped cube.
  texturedTransparency->AddChild(std::make_shared<Model>(
      Mesh::MappedCube,
      std::make_shared<StandardMaterial>(brickMaterial),
      Transform::TS(glm::vec3(-1, 0.25, 0), glm::vec3(0.5))), false);

  // Create textured quad.
  brickMaterial.rasterizerOverride.SetCullMode(RasterizerState::CullMode::OFF);
  texturedTransparency->AddChild(std::make_shared<Model>(
      Mesh::Quad,
      std::make_shared<StandardMaterial>(brickMaterial),
      Transform::TS(glm::vec3(0, 0.25, 0), glm::vec3(0.5))), false);
  brickMaterial.rasterizerOverride.ClearCullMode();

  // Create textured sphere.
  texturedTransparency->AddChild(std::make_shared<Model>(
      Mesh::Sphere,
      std::make_shared<StandardMaterial>(brickMaterial),
      Transform::TS(glm::vec3(1, 0.25, 0), glm::vec3(0.5))), false);

  // Create textured cylinder.
  texturedTransparency->AddChild(std::make_shared<Model>(
      Mesh::Cylinder,
      std::make_shared<StandardMaterial>(brickMaterial),
      Transform::TS(glm::vec3(2, 0.25, 0), glm::vec3(0.5))), false);

  // Create a spinning helix to demonstrate loading an OBJ model.
  additiveMaterial.SetDiffuse(glm::vec4(0.5f, 0.5f, 0.5f, additiveAlpha));
  spinningHelix = std::make_shared<Model>(
      Mesh::LoadOBJ("assets/models/helix.obj"),
      std::make_shared<StandardMaterial>(additiveMaterial),
      Transform::TS(glm::vec3(-1.f, 0.25f, 0.f), glm::vec3(0.2f)));
  AddChild(spinningHelix, false);

  // Create a spinning torus to demonstrate loading an OBJ model.
  spinningTorus = std::make_shared<Model>(
      Mesh::LoadOBJ("assets/models/torus.obj"),
      std::make_shared<StandardMaterial>(brickMaterial),
      Transform::TS(glm::vec3(1, 0.25, 0), glm::vec3(0.5)));
  AddChild(spinningTorus, false);

  // Create floor material.
  const int floorSize = 10;
  StandardMaterial floorMaterial = StandardMaterial::WithTexture(
      hardwoodTexture);
  floorMaterial.SetUVScale(glm::vec2((float)floorSize));

  // Create floor plane.
  AddChild(std::make_shared<Model>(
        Mesh::Quad,
        std::make_shared<StandardMaterial>(floorMaterial),
        Transform::RS(
          glm::quat(glm::radians(glm::vec3(-90, 0, 0))),
          glm::vec3(floorSize, floorSize, 1))));

  if (!enableVR) {
    // Configure camera.
    mainCamera->transform.translation = glm::vec3(0, 2, 3);
    mainCamera->LookAtPoint(glm::vec3(0));

    // Allow camera to be controller by the keyboard and mouse.
    Behavior::Attach(
        mainCamera,
        std::make_shared<KeyboardCameraController>(window));
  }
}

void dg::TransparencyScene::Update() {
  Scene::Update();

  // Slowly rotate brick cylinder and sphere.
  spinningHelix->transform.rotation = glm::quat(glm::radians(
        glm::vec3(0, dg::Time::Elapsed * -10, 0)));
  spinningTorus->transform.rotation = glm::quat(glm::radians(
        glm::vec3(0, dg::Time::Elapsed * 10, 0)));
}
