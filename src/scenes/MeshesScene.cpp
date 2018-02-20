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
#include <Lights.h>
#include <Window.h>
#include <Camera.h>
#include <Shader.h>
#include <Texture.h>
#include <Mesh.h>
#include <Model.h>
#include <Skybox.h>

std::unique_ptr<dg::MeshesScene> dg::MeshesScene::Make() {
  return std::unique_ptr<MeshesScene>(new MeshesScene(false));
}

std::unique_ptr<dg::MeshesScene> dg::MeshesScene::MakeVR() {
  return std::unique_ptr<MeshesScene>(new MeshesScene(true));
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
  skybox = std::make_shared<Skybox>(skyboxTexture);

  // Create ceiling light source.
  auto ceilingLight = std::make_shared<PointLight>(
      glm::vec3(1, 0.93, 0.86), 0.732f, 0.399f, 0.968f);
  ceilingLight->transform.translation = glm::vec3(0, 0.8, -0.5);
  AddChild(ceilingLight);

  // Container for UV meshes.
  auto uvMeshes = std::make_shared<SceneObject>(
      Transform::T(FORWARD * -1.f));
  AddChild(uvMeshes);

  // Create UV cube.
  uvMeshes->AddChild(std::make_shared<Model>(
      Mesh::Cube,
      std::make_shared<UVMaterial>(),
      Transform::TS(glm::vec3(-2, 0.25, 0), glm::vec3(0.5))), false);

  // Create UV mapped cube.
  uvMeshes->AddChild(std::make_shared<Model>(
      Mesh::MappedCube,
      std::make_shared<UVMaterial>(),
      Transform::TS(glm::vec3(-1, 0.25, 0), glm::vec3(0.5))), false);

  // Create UV quad.
  uvMeshes->AddChild(std::make_shared<Model>(
      Mesh::Quad,
      std::make_shared<UVMaterial>(),
      Transform::TS(glm::vec3(0, 0.25, 0), glm::vec3(0.5))), false);

  // Create UV sphere.
  uvMeshes->AddChild(std::make_shared<Model>(
      Mesh::Sphere,
      std::make_shared<UVMaterial>(),
      Transform::TS(glm::vec3(1, 0.25, 0), glm::vec3(0.5))), false);

  // Create UV cylinder.
  uvMeshes->AddChild(std::make_shared<Model>(
      Mesh::Cylinder,
      std::make_shared<UVMaterial>(),
      Transform::TS(glm::vec3(2, 0.25, 0), glm::vec3(0.5))), false);

  // Create shiny brick material.
  StandardMaterial brickMaterial = StandardMaterial::WithTexture(
      std::make_shared<Texture>(
        Texture::FromPath("assets/textures/brickwall.jpg")));
  brickMaterial.SetNormalMap(std::make_shared<Texture>(
      Texture::FromPath("assets/textures/brickwall_normal.jpg")));
  brickMaterial.SetSpecular(0.6f);
  brickMaterial.SetShininess(64);

  // Container for textured meshes.
  auto texturedMeshes = std::make_shared<SceneObject>(
      Transform::T(FORWARD * 1.f));
  AddChild(texturedMeshes);

  // Create textured cube.
  texturedMeshes->AddChild(std::make_shared<Model>(
      Mesh::Cube,
      std::make_shared<StandardMaterial>(brickMaterial),
      Transform::TS(glm::vec3(-2, 0.25, 0), glm::vec3(0.5))), false);

  // Create textured mapped cube.
  texturedMeshes->AddChild(std::make_shared<Model>(
      Mesh::MappedCube,
      std::make_shared<StandardMaterial>(brickMaterial),
      Transform::TS(glm::vec3(-1, 0.25, 0), glm::vec3(0.5))), false);

  // Create textured quad.
  texturedMeshes->AddChild(std::make_shared<Model>(
      Mesh::Quad,
      std::make_shared<StandardMaterial>(brickMaterial),
      Transform::TS(glm::vec3(0, 0.25, 0), glm::vec3(0.5))), false);

  // Create textured sphere.
  texturedMeshes->AddChild(std::make_shared<Model>(
      Mesh::Sphere,
      std::make_shared<StandardMaterial>(brickMaterial),
      Transform::TS(glm::vec3(1, 0.25, 0), glm::vec3(0.5))), false);

  // Create textured cylinder.
  texturedMeshes->AddChild(std::make_shared<Model>(
      Mesh::Cylinder,
      std::make_shared<StandardMaterial>(brickMaterial),
      Transform::TS(glm::vec3(2, 0.25, 0), glm::vec3(0.5))), false);

  // Create a spinning helix to demonstrate loading an OBJ model.
  spinningHelix = std::make_shared<Model>(
      Mesh::LoadOBJ("assets/models/helix.obj"),
      std::make_shared<StandardMaterial>(brickMaterial),
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

void dg::MeshesScene::Update() {
  Scene::Update();

  // Slowly rotate brick cylinder and sphere.
  spinningHelix->transform.rotation = glm::quat(glm::radians(
        glm::vec3(0, dg::Time::Elapsed * -10, 0)));
  spinningTorus->transform.rotation = glm::quat(glm::radians(
        glm::vec3(0, dg::Time::Elapsed * 10, 0)));
}

