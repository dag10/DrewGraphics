//
//  VRUtils.cpp
//

#include "dg/vr/VRUtils.h"

glm::mat4x4 dg::OVR2GLM(vr::HmdMatrix44_t hmdMat) {
  return glm::mat4x4(
    hmdMat.m[0][0], hmdMat.m[1][0], hmdMat.m[2][0], hmdMat.m[3][0],
    hmdMat.m[0][1], hmdMat.m[1][1], hmdMat.m[2][1], hmdMat.m[3][1],
    hmdMat.m[0][2], hmdMat.m[1][2], hmdMat.m[2][2], hmdMat.m[3][2],
    hmdMat.m[0][3], hmdMat.m[1][3], hmdMat.m[2][3], hmdMat.m[3][3]);
}

glm::mat4x4 dg::OVR2GLM(vr::HmdMatrix34_t hmdMat) {
  return glm::mat4x4(
    hmdMat.m[0][0], hmdMat.m[1][0], hmdMat.m[2][0], 0,
    hmdMat.m[0][1], hmdMat.m[1][1], hmdMat.m[2][1], 0,
    hmdMat.m[0][2], hmdMat.m[1][2], hmdMat.m[2][2], 0,
    hmdMat.m[0][3], hmdMat.m[1][3], hmdMat.m[2][3], 1);
}
