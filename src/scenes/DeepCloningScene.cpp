//
//  scenes/DeepCloningScene.cpp
//

#include "dg/scenes/DeepCloningScene.h"

#include <forward_list>
#include <glm/glm.hpp>
#include "dg/Camera.h"
#include "dg/EngineTime.h"
#include "dg/Lights.h"
#include "dg/Mesh.h"
#include "dg/Model.h"
#include "dg/RasterizerState.h"
#include "dg/SceneObject.h"
#include "dg/Shader.h"
#include "dg/Texture.h"
#include "dg/Window.h"
#include "dg/behaviors/KeyboardCameraController.h"
#include "dg/behaviors/KeyboardLightController.h"
#include "dg/materials/StandardMaterial.h"

using namespace dg::DeepCloning;

#pragma region Scene

std::unique_ptr<dg::DeepCloningScene> dg::DeepCloningScene::Make() {
  return std::unique_ptr<dg::DeepCloningScene>(new dg::DeepCloningScene(false));
}

std::unique_ptr<dg::DeepCloningScene> dg::DeepCloningScene::MakeVR() {
  return std::unique_ptr<dg::DeepCloningScene>(new dg::DeepCloningScene(true));
}

dg::DeepCloningScene::DeepCloningScene(bool enableVR) : Scene() {
  this->enableVR = enableVR;
}

void dg::DeepCloningScene::Initialize() {
  Scene::Initialize();

  // Lock window cursor to center.
  window->LockCursor();

  // Create sky light.
  auto skyLight = std::make_shared<DirectionalLight>(
      glm::vec3(1.0f, 0.93f, 0.86f),
      0.012f, 0.21f, 0.077f);
  skyLight->LookAtDirection(glm::normalize(glm::vec3(-0.3f, -1, -0.2f)));
  AddChild(skyLight);

  // Create floor material.
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

  // Create floor plane.
  AddChild(std::make_shared<Model>(
        dg::Mesh::Quad,
        std::make_shared<StandardMaterial>(floorMaterial),
        Transform::RS(
          glm::quat(glm::radians(glm::vec3(-90, 0, 0))),
          glm::vec3(floorSize, floorSize, 1))));

  // Configure camera.
  mainCamera->transform.translation = glm::vec3(-2.85f, 3.56f, 4.16f);
  mainCamera->LookAtDirection({ 0.30847f, -0.6647f, -0.6805f });

  // Allow camera to be controller by the keyboard and mouse.
  Behavior::Attach(
      mainCamera,
      std::make_shared<KeyboardCameraController>(window));

  // Create widgets
  const glm::vec3 colors[] = {
    glm::vec3(148, 0, 211) / 255.f,
    glm::vec3(75, 0, 130) / 255.f,
    glm::vec3(0, 0, 255) / 255.f,
    glm::vec3(0, 255, 0) / 255.f,
    glm::vec3(255, 255, 0) / 255.f,
    glm::vec3(255, 127, 0) / 255.f,
    glm::vec3(255, 0, 0) / 255.f,
  };
  int numColors = sizeof(colors) / sizeof(colors[0]);
  for (int i = 0; i < numColors; i++) {
    auto widget = BuildWidget(window, Key::NUM_1 + i, colors[i]);
    widget->transform = Transform::T({ i - (numColors * 0.5f), 0, 0 });
    widget->GetBehavior<WidgetBehavior>()->SetLight(false);
    AddChild(widget);
  }

  // Momentary buttons to show left, agnostic, and right key states for SHIFT
  // and CONTROL keys.
  {
    auto widget = BuildWidget(window, Key::LEFT_SHIFT, glm::vec3(0.4f), true);
    widget->transform = Transform::T({ -2, 0, 2 });
    widget->GetBehavior<WidgetBehavior>()->SetLight(false);
    AddChild(widget);
  }
  {
    auto widget = BuildWidget(window, Key::LEFT_CONTROL, glm::vec3(0.4f), true);
    widget->transform = Transform::T({ -2, 0, 3 });
    widget->GetBehavior<WidgetBehavior>()->SetLight(false);
    AddChild(widget);
  }
  {
    auto widget = BuildWidget(window, Key::SHIFT, glm::vec3(0.4f), true);
    widget->transform = Transform::T({ 0, 0, 2 });
    widget->GetBehavior<WidgetBehavior>()->SetLight(false);
    AddChild(widget);
  }
  {
    auto widget = BuildWidget(window, Key::CONTROL, glm::vec3(0.4f), true);
    widget->transform = Transform::T({ 0, 0, 3 });
    widget->GetBehavior<WidgetBehavior>()->SetLight(false);
    AddChild(widget);
  }
  {
    auto widget = BuildWidget(window, Key::RIGHT_SHIFT, glm::vec3(0.4f), true);
    widget->transform = Transform::T({ 2, 0, 2 });
    widget->GetBehavior<WidgetBehavior>()->SetLight(false);
    AddChild(widget);
  }
  {
    auto widget = BuildWidget(window, Key::RIGHT_CONTROL, glm::vec3(0.4f), true);
    widget->transform = Transform::T({ 2, 0, 3 });
    widget->GetBehavior<WidgetBehavior>()->SetLight(false);
    AddChild(widget);
  }
}

std::shared_ptr<dg::SceneObject> dg::DeepCloningScene::BuildWidget(
    std::shared_ptr<Window> window, Key key, glm::vec3 color, bool momentary) {
  auto widget = std::make_shared<SceneObject>();

  static std::shared_ptr<StandardMaterial> baseMaterial =
    std::make_shared<StandardMaterial>(
        StandardMaterial::WithColor(glm::vec3(0.8)));

  std::shared_ptr<StandardMaterial> buttonMaterial =
    std::make_shared<StandardMaterial>();

  std::shared_ptr<StandardMaterial> plasticMaterial =
      std::make_shared<StandardMaterial>(
          StandardMaterial::WithTransparentColor(glm::vec4(1, 1, 1, 0.5)));

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

  // Button clear plastic.
  auto plastic = std::make_shared<Model>(
        Mesh::Cube,
        plasticMaterial,
        Transform::TS(
          { 0, 0.1f, 0 },
          { 1.05f, 1.02f, 1.05f }));
  button->AddChild(plastic, false);

  // Button light
  auto light = std::make_shared<PointLight>();
  light->transform = Transform::T({ 0, 1, 0 }); // Local to button
  button->AddChild(light, false);
  Behavior::Attach(
      light,
      std::make_shared<KeyboardLightController>(window));

  // Logic
  auto logic = Behavior::Attach(
      widget,
      std::make_shared<WidgetBehavior>(window, button, light));
  logic->key = key;
  logic->color = color;
  logic->momentary = momentary;

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
  Behavior::Start();

  std::shared_ptr<Model> button = this->button.lock();
  if (!button) return;

  originalButtonPos = button->transform.translation;
}

void WidgetBehavior::Update() {
  Behavior::Update();

  std::shared_ptr<Window> window = this->window.lock();
  std::shared_ptr<Model> button = this->button.lock();
  std::shared_ptr<Light> light = this->light.lock();
  if (!window || !button || !light) return;

  if (momentary) {
    light->enabled = window->IsKeyPressed(key);
  } else if (window->IsKeyJustPressed(key)) {
    light->enabled = !light->enabled;
  }


  if (window->IsKeyPressed(key)) {
    button->transform.translation = originalButtonPos - (0.06f * UP);
  } else if (light->enabled) {
    button->transform.translation = originalButtonPos - (0.03f * UP);
  } else {
    button->transform.translation = originalButtonPos;
  }

  std::static_pointer_cast<StandardMaterial>(
      button->material)->SetDiffuse(color);

  if (light->enabled) {
    light->SetAmbient(color * 0.1f);
    light->SetDiffuse(color * 0.633f);
    light->SetSpecular(color * 0.6f);
  }
}

void WidgetBehavior::SetLight(bool enabled) {
  std::shared_ptr<Light> light = this->light.lock();
  if (!light) return;

  light->enabled = enabled;
}

#pragma endregion
