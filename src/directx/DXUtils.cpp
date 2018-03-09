//
//  MathUtils.cpp
//

#include "dg/directx/DXUtils.h"

#if defined(_DIRECTX)
// For the DirectX Math library
using namespace DirectX;
#endif

#if defined(_DIRECTX)
// Converts an XMFLOAT4X4 to a glm::mat4x4.
// Input is row-major, output is column-major.
glm::mat4x4 dg::FLOAT4X4toMAT4X4(XMFLOAT4X4 float4x4) {
  return glm::transpose(glm::make_mat4((float*)&float4x4));
}

// Converts a glm::mat4x4 to an XMFLOAT4X4.
// Input is column-major, output is row-major.
XMFLOAT4X4 dg::MAT4X4toFLOAT4X4(glm::mat4x4 mat4x4) {
  XMFLOAT4X4 ret;
  glm::mat4x4 transposed = glm::transpose(mat4x4);
  const float *values = (const float*)glm::value_ptr(transposed);
  memcpy(&ret, glm::value_ptr(transposed), sizeof(ret));
  return ret;
}
#endif

