//
//  behaviors/KeyboardLightController.cpp
//

#include <behaviors/KeyboardLightController.h>
#include <EngineTime.h>
#include <Lights.h>
#include <Window.h>
#include <InputCodes.h>
#include <iostream>

dg::KeyboardLightController::KeyboardLightController(
  std::weak_ptr<Window> window) : window(window), Behavior() {}

dg::KeyboardLightController::KeyboardLightController(
    std::weak_ptr<Window> window,
    Key ambientModifierKey, Key diffuseModifierKey, Key specularModifierKey)
  : window(window), ambientModifierKey(ambientModifierKey),
    diffuseModifierKey(diffuseModifierKey),
    specularModifierKey(specularModifierKey),
    Behavior() {}

void dg::KeyboardLightController::Update() {
  Behavior::Update();

  std::shared_ptr<Light> light =
    std::dynamic_pointer_cast<Light>(SceneObject());
  std::shared_ptr<Window> window = this->window.lock();
  if (!light || !window) return;

  const float lightDelta = 0.05f;
  glm::vec3 ambient = light->GetAmbient();
  if (window->IsKeyPressed(ambientModifierKey) &&
      window->IsKeyJustPressed(Key::UP)) {
    ambient += ambient * lightDelta;
    std::cout << "Ambient R: " << ambient.r << std::endl;
  } else if (window->IsKeyPressed(ambientModifierKey) &&
      window->IsKeyJustPressed(Key::DOWN)) {
    ambient -= ambient * lightDelta;
    light->SetAmbient(ambient - ambient * lightDelta);
    std::cout << "Ambient R: " << ambient.r << std::endl;
  }
  light->SetAmbient(ambient);

  // Adjust light diffuse power with keyboard.
  glm::vec3 diffuse = light->GetDiffuse();
  if (window->IsKeyPressed(diffuseModifierKey) &&
      window->IsKeyJustPressed(Key::UP)) {
    diffuse += diffuse * lightDelta;
    std::cout << "Diffuse R: " << diffuse.r << std::endl;
  } else if (window->IsKeyPressed(diffuseModifierKey) &&
      window->IsKeyJustPressed(Key::DOWN)) {
    diffuse -= diffuse * lightDelta;
    std::cout << "Diffuse R: " << diffuse.r << std::endl;
  }
  light->SetDiffuse(diffuse);

  // Adjust light specular power with keyboard.
  glm::vec3 specular = light->GetSpecular();
  if (window->IsKeyPressed(specularModifierKey) &&
      window->IsKeyJustPressed(Key::UP)) {
    specular += specular * lightDelta;
    std::cout << "Specular R: " << specular.r << std::endl;
  } else if (window->IsKeyPressed(specularModifierKey) &&
      window->IsKeyJustPressed(Key::DOWN)) {
    specular -= specular * lightDelta;
    std::cout << "Specular R: " << specular.r << std::endl;
  }
  light->SetSpecular(specular);
}
