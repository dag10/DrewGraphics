//
//  Camera.cpp
//

#include <Camera.h>

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/constants.hpp>
#include <MathUtils.h>

dg::Camera::Camera() : SceneObject() {}

glm::mat4x4 dg::Camera::GetViewMatrix() const {
  return SceneSpace().Inverse().ToMat4();
}

glm::mat4x4 dg::Camera::GetViewMatrix(vr::EVREye eye) const {
  glm::mat4x4 head2eye = HmdMat2Glm(
    vr::VRSystem()->GetEyeToHeadTransform(eye));
  return glm::inverse(head2eye) * GetViewMatrix();
}

glm::mat4x4 dg::Camera::GetProjectionMatrix(float aspectRatio) const {
  return glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
}

glm::mat4x4 dg::Camera::GetProjectionMatrix(vr::EVREye eye) const {
  return HmdMat2Glm(
    vr::VRSystem()->GetProjectionMatrix(eye, nearClip, farClip));
}

