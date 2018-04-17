//
//  Camera.cpp
//

#include "dg/Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/constants.hpp>
#include <glm/gtx/quaternion.hpp>
#include "dg/directx/DXUtils.h"
#include "dg/vr/VRUtils.h"

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
  glm::mat4x4 head2eye = OVR2GLM(
    vr::VRSystem()->GetEyeToHeadTransform(eye));
  return glm::inverse(head2eye) * GetViewMatrix();
}

glm::mat4x4 dg::Camera::GetProjectionMatrix() const {
#if defined(_OPENGL)
  switch (projection) {
    case Projection::Perspective:
      return glm::perspective(fov, aspectRatio, nearClip, farClip);
    case Projection::Orthographic:
      return glm::orthoRH(-orthoWidth * 0.5f, orthoWidth * 0.5f,
                          -orthoHeight * 0.5f, orthoHeight * 0.5f, nearClip,
                          farClip);
  }
#elif defined(_DIRECTX)
  XMFLOAT4X4 ret;
  switch (projection) {
    case Projection::Perspective:
      XMStoreFloat4x4(
          &ret, XMMatrixPerspectiveFovRH(fov, aspectRatio, nearClip, farClip));
      break;
    case Projection::Orthographic:
      XMStoreFloat4x4(&ret, XMMatrixOrthographicRH(orthoWidth, orthoHeight,
                                                   nearClip, farClip));
      break;
  }
  return glm::transpose(FLOAT4X4toMAT4X4(ret));
#endif
}

glm::mat4x4 dg::Camera::GetProjectionMatrix(vr::EVREye eye) const {
  return OVR2GLM(
    vr::VRSystem()->GetProjectionMatrix(eye, nearClip, farClip));
}
