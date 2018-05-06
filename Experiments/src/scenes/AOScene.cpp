//
//  scenes/AOScene.cpp
//

#include "dg/scenes/AOScene.h"

#include <forward_list>
#include <glm/glm.hpp>
#include "dg/Camera.h"
#include "dg/Graphics.h"
#include "dg/Lights.h"
#include "dg/Mesh.h"
#include "dg/Model.h"
#include "dg/Shader.h"
#include "dg/Skybox.h"
#include "dg/Texture.h"
#include "dg/Window.h"
#include "dg/behaviors/KeyboardCameraController.h"
#include "dg/behaviors/KeyboardLightController.h"
#include "dg/materials/StandardMaterial.h"
#include "dg/materials/UVMaterial.h"

std::unique_ptr<dg::AOScene> dg::AOScene::Make() {
  return std::unique_ptr<dg::AOScene>(new dg::AOScene());
}

dg::AOScene::AOScene() : Scene() {}

void dg::AOScene::Initialize() {
  Scene::Initialize();

  // Lock window cursor to center.
  window->LockCursor();

  // Create skybox.
  skybox =
      Skybox::Create(Texture::FromPath("assets/textures/skybox_daylight.png"));

  // Create sky light.
  auto skyLight = std::make_shared<DirectionalLight>(
      glm::vec3(1.0f, 0.93f, 0.86f), 0.44, 1.17, 0.07f);
  skyLight->LookAtDirection(glm::normalize(glm::vec3(-0.3f, -1, -0.2f)));
  Behavior::Attach(skyLight, std::make_shared<KeyboardLightController>(window));
  AddChild(skyLight);

  // Create model material.
  StandardMaterial cubeMaterial = StandardMaterial::WithColor(glm::vec3(0.5));
  cubeMaterial.SetSpecular(glm::vec3(0));

  // Load model.
  AddChild(std::make_shared<Model>(
      Mesh::LoadOBJ("assets/models/crytek-sponza/sponza.obj"),
      //std::make_shared<StandardMaterial>(cubeMaterial),
      std::make_shared<UVMaterial>(),
      Transform::S(glm::vec3(0.0025))));

  // Create floor material.
  const int floorSize = 2;
  StandardMaterial floorMaterial = StandardMaterial::WithColor(glm::vec3(0.2f));
  floorMaterial.SetUVScale(glm::vec2((float)floorSize));
  floorMaterial.SetSpecular(glm::vec3(1));
  floorMaterial.SetShininess(64);

  // Configure camera.
  cameras.main->transform.translation = glm::vec3(-3.11, 1.75, 0.23);
  cameras.main->LookAtDirection(glm::vec3(0.9713, -0.198, -0.132));

  // Allow camera to be controller by the keyboard and mouse.
  Behavior::Attach(cameras.main,
                   std::make_shared<KeyboardCameraController>(window));
}
