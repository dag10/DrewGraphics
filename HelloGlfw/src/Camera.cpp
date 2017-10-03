//
//  Camera.cpp
//

#include <Camera.h>
#include <Transform.h>

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/constants.hpp>

void dg::Camera::LookAtDirection(glm::vec3 direction) {
  direction = glm::normalize(direction);

  // If we're told to look straight up or down, we have to pick a yaw.
  // Let's just default to looking at the global forward direction.
  // TODO: Retain the previous yaw in this case.
  if (direction.y == 1 || direction.y == -1) {
    direction = glm::normalize(glm::vec3(0, direction.y, -0.0001f));
  }

  glm::vec3 eulerOrientation(0);

  float horizontalLen = glm::length(glm::vec2(direction.x, direction.z));
  eulerOrientation.x = atan(direction.y / horizontalLen);

  eulerOrientation.y = -atan(direction.x / -direction.z);
  if (direction.z >= 0) {
    eulerOrientation.y += glm::pi<float>();
  }

  transform.rotation = glm::quat(eulerOrientation);
}

void dg::Camera::LookAtPoint(glm::vec3 target) {
  LookAtDirection(target - transform.translation);
}

void dg::Camera::OrientUpwards() {
  LookAtDirection(transform.Forward());
}

glm::mat4x4 dg::Camera::GetViewMatrix() const {
  return transform.Inverse().ToMat4();
}

glm::mat4x4 dg::Camera::GetProjectionMatrix(float aspectRatio) const {
  return glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
}

