//
//  Camera.cpp
//

#include <Camera.h>

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/constants.hpp>

dg::Camera::Camera() : SceneObject() {}

glm::mat4x4 dg::Camera::GetViewMatrix() const {
  return transform.Inverse().ToMat4();
}

glm::mat4x4 dg::Camera::GetProjectionMatrix(float aspectRatio) const {
  return glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
}

