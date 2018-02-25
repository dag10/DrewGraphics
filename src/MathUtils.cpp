//
//  MathUtils.cpp
//

#include "dg/MathUtils.h"

#if defined(_DIRECTX)
// For the DirectX Math library
using namespace DirectX;
#endif

glm::mat4x4 dg::HmdMat2Glm(vr::HmdMatrix44_t hmdMat) {
  return glm::mat4x4(
    hmdMat.m[0][0], hmdMat.m[1][0], hmdMat.m[2][0], hmdMat.m[3][0],
    hmdMat.m[0][1], hmdMat.m[1][1], hmdMat.m[2][1], hmdMat.m[3][1],
    hmdMat.m[0][2], hmdMat.m[1][2], hmdMat.m[2][2], hmdMat.m[3][2],
    hmdMat.m[0][3], hmdMat.m[1][3], hmdMat.m[2][3], hmdMat.m[3][3]);
}

glm::mat4x4 dg::HmdMat2Glm(vr::HmdMatrix34_t hmdMat) {
  return glm::mat4x4(
    hmdMat.m[0][0], hmdMat.m[1][0], hmdMat.m[2][0], 0,
    hmdMat.m[0][1], hmdMat.m[1][1], hmdMat.m[2][1], 0,
    hmdMat.m[0][2], hmdMat.m[1][2], hmdMat.m[2][2], 0,
    hmdMat.m[0][3], hmdMat.m[1][3], hmdMat.m[2][3], 1);
}

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

