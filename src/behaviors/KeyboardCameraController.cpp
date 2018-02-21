//
//  behaviors/KeyboardCameraController.cpp
//

#include <behaviors/KeyboardCameraController.h>
#include <EngineTime.h>
#include <iostream>

dg::KeyboardCameraController::KeyboardCameraController(
    std::weak_ptr<Window> window) : window(window) {}

dg::KeyboardCameraController::KeyboardCameraController(
    std::weak_ptr<Window> window, float speed) : window(window), speed(speed) {}

void dg::KeyboardCameraController::Start() {
  Behavior::Start();

  auto sceneObject = this->sceneObject.lock();
  if (!sceneObject) return;

  originalTransform = sceneObject->transform;
}

void dg::KeyboardCameraController::Update() {
  Behavior::Update();

  auto sceneObject = this->GetSceneObject();
  auto window = this->window.lock();
  if (!sceneObject || !window) return;

  const float rotationSpeed = 90; // degrees per second
  const float cursorRotationSpeed = 0.3f; // degrees per cursor pixels moved

  // If C is tapped, print out the sceneObject position and orientation.
  if (window->IsKeyJustPressed(Key::C)) {
    std::cout << std::endl << "Camera position:" << std::endl;
    std::cout << sceneObject->transform << std::endl;
    glm::vec3 dir = sceneObject->transform.Forward();
    std::cout << "Forward: ";
    std::cout << dir.x << ", ";
    std::cout << dir.y << ", ";
    std::cout << dir.z << std::endl;
  }

  // If R is tapped, just reset the sceneObject position.
  if (window->IsKeyJustPressed(Key::R)) {
    sceneObject->transform = originalTransform;
    return;
  }

  // Calculate new rotation for sceneObject based on mouse.
  if (window->IsCursorLocked()) {
    glm::vec2 cursorDelta = window->GetCursorDelta();
    glm::quat pitch = glm::quat(glm::radians(glm::vec3(
            -cursorDelta.y * cursorRotationSpeed,
            0,
            0)));
    glm::quat yaw = glm::quat(glm::radians(glm::vec3(
            0,
            -cursorDelta.x * cursorRotationSpeed,
            0)));
    glm::quat rotation = yaw * sceneObject->transform.rotation *
      pitch * glm::inverse(sceneObject->transform.rotation);
    sceneObject->LookAtDirection(rotation * sceneObject->transform.Forward());
  }

  // Calculate new movement relative to sceneObject, based on WASD keys.
  glm::vec3 movementDir(0);
  if (window->IsKeyPressed(Key::W)) {
    movementDir += FORWARD;
  }
  if (window->IsKeyPressed(Key::S)) {
    movementDir += -FORWARD;
  }
  if (window->IsKeyPressed(Key::A)) {
    movementDir += -RIGHT;
  }
  if (window->IsKeyPressed(Key::D)) {
    movementDir += RIGHT;
  }
  float speedMultiplier =
    (window->IsKeyPressed(Key::LEFT_SHIFT)) ? 2.f : 1.f;
  dg::Transform xfDelta = dg::Transform::T(
      movementDir * speed * speedMultiplier * (float)Time::Delta);

  sceneObject->transform = sceneObject->transform * xfDelta;
}

