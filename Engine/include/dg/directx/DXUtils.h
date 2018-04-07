//
//  DXUtils.h
//
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#if defined(_DIRECTX)
#include <DirectXMath.h>
#endif

namespace dg {

#if defined(_DIRECTX)
  // Converts an XMFLOAT4X4 to a glm::mat4x4.
  // Input is row-major, output is column-major.
  glm::mat4x4 FLOAT4X4toMAT4X4(DirectX::XMFLOAT4X4 float4x4);

  // Converts a glm::mat4x4 to an XMFLOAT4X4.
  // Input is column-major, output is row-major.
  DirectX::XMFLOAT4X4 MAT4X4toFLOAT4X4(glm::mat4x4 mat4x4);
#endif

}  // namespace dg
