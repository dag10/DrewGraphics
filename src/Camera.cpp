//
//  Camera.cpp
//

#include <Camera.h>

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/constants.hpp>
#include <MathUtils.h>

#if defined(_DIRECTX)
#include <DirectXMath.h>

// For the DirectX Math library
using namespace DirectX;
#endif


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
#if defined(_OPENGL)
  return glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
#elif defined(_DIRECTX)
  XMFLOAT4X4 ret;
  XMStoreFloat4x4(&ret, XMMatrixPerspectiveFovRH(
    glm::radians(fov), aspectRatio, nearClip, farClip));
  return glm::transpose(FLOAT4X4toMAT4X4(ret));
#endif
}

glm::mat4x4 dg::Camera::GetProjectionMatrix(vr::EVREye eye) const {
  return HmdMat2Glm(
    vr::VRSystem()->GetProjectionMatrix(eye, nearClip, farClip));
}

