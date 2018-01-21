//
//  scenes/LayoutScene.h
//

#include <scenes/LayoutScene.h>

#include <glm/glm.hpp>
#include <materials/StandardMaterial.h>
#include <behaviors/KeyboardCameraController.h>
#include <lights/DirectionalLight.h>

std::unique_ptr<dg::LayoutScene> dg::LayoutScene::Make() {
  return std::unique_ptr<dg::LayoutScene>(new dg::LayoutScene());
}

dg::LayoutScene::LayoutScene() : Scene() {}

void dg::LayoutScene::Initialize() {
  // Lock window cursor to center.
  window->LockCursor();

  // Create skybox.
  skybox = std::unique_ptr<Skybox>(new Skybox(std::make_shared<Texture>(
          Texture::FromPath("assets/textures/skybox_daylight.png"))));

  // Create directinal light.
  auto directionalLight = std::make_shared<DirectionalLight>(
      glm::normalize(glm::vec3(0, -1, -0.3)),
      glm::vec3(1.0f, 0.93f, 0.86f),
      0.732f, 0.399f, 0.968f);
  AddChild(directionalLight);

  // Create floor material.
  glm::vec2 floorSize(12, 28);
  Material floorMaterial;
  floorMaterial.shader = std::make_shared<Shader>(Shader::FromFiles(
        "assets/shaders/checkerboard.v.glsl",
        "assets/shaders/checkerboard.f.glsl"));
  floorMaterial.SetProperty("_Size", floorSize);
  floorMaterial.SetProperty("_Colors[0]", glm::vec3(1, 0, 0));
  floorMaterial.SetProperty("_Colors[1]", glm::vec3(1, 1, 0));

  // Create floor plane.
  AddChild(std::make_shared<Model>(
        dg::Mesh::Quad,
        std::make_shared<Material>(floorMaterial),
        Transform::RS(
          glm::quat(glm::radians(glm::vec3(-90, 0, 0))),
          glm::vec3(floorSize.x, floorSize.y, 1))));

  // Create sphere material.
  StandardMaterial sphereMaterial = StandardMaterial::WithColor(glm::vec3(0.3));
  sphereMaterial.SetSpecular(0.3);
  sphereMaterial.SetShininess(64);

  // Create front sphere.
  AddChild(std::make_shared<Model>(
      dg::Mesh::Sphere,
      std::make_shared<StandardMaterial>(sphereMaterial),
      Transform::TS(glm::vec3(-3, 2, 0), glm::vec3(2.5))));

  // Create back sphere.
  AddChild(std::make_shared<Model>(
      dg::Mesh::Sphere,
      std::make_shared<StandardMaterial>(sphereMaterial),
      Transform::TS(glm::vec3(-1, 1.5, -2), glm::vec3(2.5))));

  // Create camera.
  mainCamera = std::make_shared<Camera>();
  mainCamera->transform.translation = glm::vec3(-3, 2, 5);
  mainCamera->LookAtPoint(glm::vec3(-3, 0, -7));
  AddChild(mainCamera);

  // Allow camera to be controller by the keyboard and mouse.
  Behavior::Attach(
      mainCamera,
      std::make_shared<KeyboardCameraController>(window, 4));
}

