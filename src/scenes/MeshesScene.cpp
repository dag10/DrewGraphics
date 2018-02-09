//
//  scenes/MeshesScene.h
//

#include <scenes/MeshesScene.h>

#include <iostream>
#include <glm/glm.hpp>
#include <EngineTime.h>
#include <forward_list>
#include <materials/StandardMaterial.h>
#include <materials/UVMaterial.h>
#include <behaviors/KeyboardCameraController.h>
#include <lights/PointLight.h>

std::unique_ptr<dg::MeshesScene> dg::MeshesScene::Make() {
  return std::unique_ptr<dg::MeshesScene>(new dg::MeshesScene(false));
}

std::unique_ptr<dg::MeshesScene> dg::MeshesScene::MakeVR() {
  return std::unique_ptr<dg::MeshesScene>(new dg::MeshesScene(true));
}

dg::MeshesScene::MeshesScene(bool enableVR) : Scene() {
  this->enableVR = enableVR;
}

void dg::MeshesScene::Initialize() {
  Scene::Initialize();

  std::cout
    << "This scene is a demo of various built-in meshes." << std::endl
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
  std::shared_ptr<Texture> hardwoodTexture = std::make_shared<Texture>(
      Texture::FromPath("assets/textures/hardwood.jpg"));
  std::shared_ptr<Texture> skyboxTexture = std::make_shared<Texture>(
      Texture::FromPath("assets/textures/skybox_daylight.png"));

  // Create skybox.
  skybox = std::unique_ptr<Skybox>(new Skybox(skyboxTexture));

  // Create ceiling light source.
  auto ceilingLight = std::make_shared<PointLight>(
      glm::vec3(1, 0.93, 0.86), 0.732f, 0.399f, 0.968f);
  ceilingLight->transform.translation = glm::vec3(0, 0.8, 0.5);
  AddChild(ceilingLight);

  // Container for meshes.
  auto meshes = std::make_shared<SceneObject>();
  AddChild(meshes);

  // Create cube.
  auto cube = std::make_shared<Model>(
      dg::Mesh::Cube,
      std::make_shared<UVMaterial>(),
      Transform::TS(glm::vec3(-2, 0.25, 0), glm::vec3(0.5)));
  meshes->AddChild(cube, false);

  // Create mapped cube.
  auto mappedcube = std::make_shared<Model>(
      dg::Mesh::MappedCube,
      std::make_shared<UVMaterial>(),
      Transform::TS(glm::vec3(-1, 0.25, 0), glm::vec3(0.5)));
  meshes->AddChild(mappedcube, false);

  // Create quad.
  auto quad = std::make_shared<Model>(
      dg::Mesh::Quad,
      std::make_shared<UVMaterial>(),
      Transform::TS(glm::vec3(0, 0.25, 0), glm::vec3(0.5)));
  meshes->AddChild(quad, false);

  // Create sphere.
  auto sphere = std::make_shared<Model>(
      dg::Mesh::Sphere,
      std::make_shared<UVMaterial>(),
      Transform::TS(glm::vec3(1, 0.25, 0), glm::vec3(0.5)));
  meshes->AddChild(sphere, false);

  // Create cylinder.
  auto cylinder = std::make_shared<Model>(
      dg::Mesh::Cylinder,
      std::make_shared<UVMaterial>(),
      Transform::TS(glm::vec3(2, 0.25, 0), glm::vec3(0.5)));
  meshes->AddChild(cylinder, false);

  // Create floor material.
  const int floorSize = 10;
  StandardMaterial floorMaterial = StandardMaterial::WithTexture(
      hardwoodTexture);
  floorMaterial.SetUVScale(glm::vec2((float)floorSize));

  // Create shiny brick material.
  StandardMaterial brickMaterial = StandardMaterial::WithTexture(
      std::make_shared<Texture>(
        Texture::FromPath("assets/textures/brickwall.jpg")));
  brickMaterial.SetNormalMap(std::make_shared<Texture>(
      Texture::FromPath("assets/textures/brickwall_normal.jpg")));
  brickMaterial.SetSpecular(0.6f);
  brickMaterial.SetShininess(64);

  // Create a sphere made out of shiny brick.
  texturedSphere = std::make_shared<Model>(
      dg::Mesh::Sphere,
      std::make_shared<StandardMaterial>(brickMaterial),
      Transform::TS(glm::vec3(-1, 0.25, 1), glm::vec3(0.5)));
  meshes->AddChild(texturedSphere, false);

  // Create a cylinder made out of shiny brick.
  texturedCylinder = std::make_shared<Model>(
      dg::Mesh::Cylinder,
      std::make_shared<StandardMaterial>(brickMaterial),
      Transform::TS(glm::vec3(1, 0.25, 1), glm::vec3(0.5)));
  meshes->AddChild(texturedCylinder, false);

  // Create floor plane.
  AddChild(std::make_shared<Model>(
        dg::Mesh::Quad,
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

void dg::MeshesScene::Update() {
  Scene::Update();

  // Slowly rotate brick cylinder and sphere.
  texturedSphere->transform.rotation = glm::quat(glm::radians(
        glm::vec3(0, dg::Time::Elapsed * -10, 0)));
  texturedCylinder->transform.rotation = glm::quat(glm::radians(
        glm::vec3(0, dg::Time::Elapsed * 10, 0)));
}

