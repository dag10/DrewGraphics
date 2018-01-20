//
//  scenes/MeshesScene.h
//

#include <scenes/MeshesScene.h>

#include <glm/glm.hpp>
#include <EngineTime.h>
#include <forward_list>
#include <materials/StandardMaterial.h>
#include <materials/UVMaterial.h>
#include <behaviors/KeyboardCameraController.h>
#include <lights/PointLight.h>

std::unique_ptr<dg::MeshesScene> dg::MeshesScene::Make() {
  return std::unique_ptr<dg::MeshesScene>(new dg::MeshesScene());
}

dg::MeshesScene::MeshesScene() : Scene() {}

void dg::MeshesScene::Initialize() {
  // Lock window cursor to center.
  window->LockCursor();

  // Create textures.
  std::shared_ptr<Texture> hardwoodTexture = std::make_shared<Texture>(
      Texture::FromPath("assets/textures/hardwood.jpg"));
  std::shared_ptr<Texture> skyboxTexture = std::make_shared<Texture>(
      Texture::FromPath("assets/textures/skybox_daylight.png"));

  // Create skybox.
  skybox = std::unique_ptr<Skybox>(new Skybox(skyboxTexture));

  // Container for meshes.
  auto meshes = std::make_shared<SceneObject>();
  meshes->transform.translation.x = 0.5;
  AddChild(meshes);

  // Create ceiling light source.
  auto ceilingLight = std::make_shared<PointLight>(
      glm::vec3(1.0, 0.93, 0.86), 0.732, 0.399, 0.968);
  ceilingLight->transform.translation = glm::vec3(0.8, 1.2, -0.2);
  meshes->AddChild(ceilingLight, false);

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

  // Create cylinder.
  auto cylinder = std::make_shared<Model>(
      dg::Mesh::Cylinder,
      std::make_shared<UVMaterial>(),
      Transform::TS(glm::vec3(1, 0.25, 0), glm::vec3(0.5)));
  meshes->AddChild(cylinder, false);

  // Create floor material.
  const int floorSize = 10;
  StandardMaterial floorMaterial = StandardMaterial::WithTexture(
      hardwoodTexture);
  floorMaterial.SetUVScale(glm::vec2(floorSize));

  // Create floor plane.
  AddChild(std::make_shared<Model>(
        dg::Mesh::Quad,
        std::make_shared<StandardMaterial>(floorMaterial),
        Transform::RS(
          glm::quat(glm::radians(glm::vec3(-90, 0, 0))),
          glm::vec3(floorSize, floorSize, 1))));

  // Create camera.
  mainCamera = std::make_shared<Camera>();
  mainCamera->transform.translation = glm::vec3(0, 2, 3);
  mainCamera->LookAtPoint(glm::vec3(0));
  AddChild(mainCamera);

  // Allow camera to be controller by the keyboard and mouse.
  behaviors.push_back(std::unique_ptr<Behavior>(
        new KeyboardCameraController(mainCamera, window)));
}

