//
//  MathUtils.h
//
#pragma once

#include <openvr.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <DirectXMath.h>

namespace dg {

  glm::mat4x4 HmdMat2Glm(vr::HmdMatrix44_t hmdMat);

  glm::mat4x4 HmdMat2Glm(vr::HmdMatrix34_t hmdMat);

  // Converts an XMFLOAT4X4 to a glm::mat4x4.
  // Input is row-major, output is column-major.
  glm::mat4x4 FLOAT4X4toMAT4X4(DirectX::XMFLOAT4X4 float4x4);

  // Converts a glm::mat4x4 to an XMFLOAT4X4.
  // Input is column-major, output is row-major.
  DirectX::XMFLOAT4X4 MAT4X4toFLOAT4X4(glm::mat4x4 mat4x4);

}  // namespace dg
