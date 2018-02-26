//
//  Camera.cpp
//

#include "dg/Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/constants.hpp>
#include "dg/MathUtils.h"

#include <DirectXMath.h>

// For the DirectX Math library
using namespace DirectX;


dg::Camera::Camera() : SceneObject() {}

glm::mat4x4 dg::Camera::GetViewMatrix() const {
  return SceneSpace().Inverse().ToMat4();
}

glm::mat4x4 dg::Camera::GetProjectionMatrix(float aspectRatio) const {
  XMFLOAT4X4 ret;
  XMStoreFloat4x4(&ret, XMMatrixPerspectiveFovRH(
    glm::radians(fov), aspectRatio, nearClip, farClip));
  return glm::transpose(FLOAT4X4toMAT4X4(ret));
}
