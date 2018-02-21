//
//  scenes/DeepCloningScene.cpp
//

#include <scenes/DeepCloningScene.h>

#include <glm/glm.hpp>
#include <EngineTime.h>
#include <forward_list>
#include <materials/StandardMaterial.h>
#include <behaviors/KeyboardCameraController.h>
#include <behaviors/KeyboardLightController.h>
#include <Lights.h>
#include <SceneObject.h>
#include <Window.h>
#include <Camera.h>
#include <Shader.h>
#include <Texture.h>
#include <Mesh.h>
#include <Model.h>

using namespace dg::DeepCloning;

#pragma region Scene

std::unique_ptr<dg::DeepCloningScene> dg::DeepCloningScene::Make() {
  return std::unique_ptr<dg::DeepCloningScene>(new dg::DeepCloningScene());
}

dg::DeepCloningScene::DeepCloningScene() : Scene() {}

void dg::DeepCloningScene::Initialize() {
  Scene::Initialize();

  // Lock window cursor to center.
  window->LockCursor();

  // Create sky light.
  auto skyLight = std::make_shared<DirectionalLight>(
      glm::vec3(1.0f, 0.93f, 0.86f),
      0.072f, 0.31f, 0.077f);
  skyLight->LookAtDirection(glm::normalize(glm::vec3(-0.3f, -1, -0.2f)));
  AddChild(skyLight);

  // Create floor material.
  const int floorSize = 10;
  StandardMaterial floorMaterial = StandardMaterial::WithTexture(
      std::make_shared<Texture>(Texture::FromPath(
          "assets/textures/Flooring_Stone_001/Flooring_Stone_001_COLOR.png")));
  floorMaterial.SetNormalMap(
      std::make_shared<Texture>(Texture::FromPath(
          "assets/textures/Flooring_Stone_001/Flooring_Stone_001_NRM.png")));
  floorMaterial.SetSpecular(
      std::make_shared<Texture>(Texture::FromPath(
          "assets/textures/Flooring_Stone_001/Flooring_Stone_001_SPEC.png")));
  floorMaterial.SetShininess(9);
  floorMaterial.SetUVScale(glm::vec2((float)floorSize));
  floorMaterial.SetLit(true);

  // Create floor plane.
  AddChild(std::make_shared<Model>(
        dg::Mesh::Quad,
        std::make_shared<StandardMaterial>(floorMaterial),
        Transform::RS(
          glm::quat(glm::radians(glm::vec3(-90, 0, 0))),
          glm::vec3(floorSize, floorSize, 1))));

  // Configure camera.
  mainCamera->transform.translation = glm::vec3(-1.25f, 2, 1.1f);
  mainCamera->LookAtPoint({ 0, 0, 0 });

  // Allow camera to be controller by the keyboard and mouse.
  Behavior::Attach(
      mainCamera,
      std::make_shared<KeyboardCameraController>(window));

  // Create widgets
  auto widget = BuildWidget(window);
  AddChild(widget);
}

std::shared_ptr<dg::SceneObject> dg::DeepCloningScene::BuildWidget(
    std::shared_ptr<Window> window) {
  auto widget = std::make_shared<SceneObject>();

  glm::vec3 color(0, 0, 1);

  static std::shared_ptr<StandardMaterial> baseMaterial =
    std::make_shared<StandardMaterial>(
        //StandardMaterial::WithTexture(std::make_shared<Texture>(
            //Texture::FromPath("assets/textures/rustyplate.jpg"))));
        StandardMaterial::WithColor(glm::vec3(0.8)));

  std::shared_ptr<StandardMaterial> buttonMaterial =
    std::make_shared<StandardMaterial>(
        StandardMaterial::WithColor(color * 1.0f));
  //buttonMaterial->SetLit(false);

  // Base of button
  widget->AddChild(std::make_shared<Model>(
        Mesh::Cube,
        baseMaterial,
        Transform::S({ 0.5f, 0.3f, 0.5f })));

  // Button itself
  auto button = std::make_shared<Model>(
        Mesh::Cube,
        buttonMaterial,
        Transform::TS(
          { 0, 0.1f, 0 },
          { 0.35f, 0.3f, 0.35f }));
  widget->AddChild(button, false);

  // Button light
  auto light = std::make_shared<PointLight>(color, 0.1f, 0.633f, 0.6f);
  light->transform = Transform::T({ 0, 1, 0 }); // Local to button
  button->AddChild(light, false);
  Behavior::Attach(
      light,
      std::make_shared<KeyboardLightController>(window));

  // Logic
  Behavior::Attach(
      widget,
      std::make_shared<WidgetBehavior>(window, button, light));
  widget->GetBehavior<WidgetBehavior>()->key = Key::SPACE;
  // TODO: Actually just have Behavior::Attach() return itself.

  return widget;
}

#pragma endregion
#pragma region WidgetBehavior

dg::DeepCloning::WidgetBehavior::WidgetBehavior(
    std::weak_ptr<Window> window,
    std::weak_ptr<Model> button, std::weak_ptr<Light> light)
  : window(window), button(button), light(light), Behavior() {
}

void WidgetBehavior::Start() {
}

void WidgetBehavior::Update() {
  std::shared_ptr<Window> window = this->window.lock();
  std::shared_ptr<Model> button = this->button.lock();
  std::shared_ptr<Light> light = this->light.lock();
  if (!window || !button || !light) return;

  if (window->IsKeyJustPressed(key)) {
    light->enabled = !light->enabled;
  }
}

#pragma endregion

