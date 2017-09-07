//
//  Camera.cpp
//

#include "Camera.h"
#include "Transform.h"

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/constants.hpp>

void dg::Camera::LookAtDirection(glm::vec3 direction) {
  glm::vec3 eulerOrientation(0);

  float horizontalLen = glm::length2(glm::vec2(direction.x, direction.z));
  eulerOrientation.x = atan(direction.y / horizontalLen);

  eulerOrientation.y = -atan(direction.x / -direction.z);
  if (direction.z >= 0) {
    eulerOrientation.y += glm::pi<float>();
  }

  transform.rotation = glm::quat(eulerOrientation);
}

void dg::Camera::LookAtPoint(glm::vec3 target) {
  LookAtDirection(glm::normalize(target - transform.translation));
}

glm::mat4x4 dg::Camera::GetViewMatrix() const {
  return transform.Inverse().ToMat4();
}

glm::mat4x4 dg::Camera::GetProjectionMatrix(float aspectRatio) const {
  return glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
}

