//
//  KeyboardCameraController.cpp
//

#include <behaviors/KeyboardCameraController.h>
#include <EngineTime.h>

dg::KeyboardCameraController::KeyboardCameraController(
    std::weak_ptr<Camera> camera, std::weak_ptr<Window> window)
  : camera(camera), window(window) {}

void dg::KeyboardCameraController::Update() {
  auto camera = this->camera.lock();
  auto window = this->window.lock();
  if (!camera || !window) return;

  const float speed = 1.8f; // units per second
  const float rotationSpeed = 90; // degrees per second
  const float cursorRotationSpeed = 0.3f; // degrees per cursor pixels moved

  // Calculate new rotation for camera based on mouse.
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
    glm::quat rotation = yaw * camera->transform.rotation *
      pitch * glm::inverse(camera->transform.rotation);
    camera->LookAtDirection(rotation * camera->transform.Forward());
  }

  // Calculate new movement relative to camera, based on WASD keys.
  glm::vec3 movementDir(0);
  if (window->IsKeyPressed(GLFW_KEY_W)) {
    movementDir += FORWARD;
  }
  if (window->IsKeyPressed(GLFW_KEY_S)) {
    movementDir += -FORWARD;
  }
  if (window->IsKeyPressed(GLFW_KEY_A)) {
    movementDir += -RIGHT;
  }
  if (window->IsKeyPressed(GLFW_KEY_D)) {
    movementDir += RIGHT;
  }
  float speedMultiplier =
    (window->IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) ? 2.f : 1.f;
  dg::Transform xfDelta = dg::Transform::T(
      movementDir * speed * speedMultiplier * (float)Time::Delta);

  camera->transform = camera->transform * xfDelta;
}

