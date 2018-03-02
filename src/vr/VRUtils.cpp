//
//  VRUtils.cpp
//

#include "dg/vr/VRUtils.h"

glm::mat4x4 dg::OVR2GLM(vr::HmdMatrix44_t mat) {
  return glm::mat4x4(
    mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
    mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
    mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
    mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]);
}

glm::mat4x4 dg::OVR2GLM(vr::HmdMatrix34_t mat) {
  return glm::mat4x4(
    mat.m[0][0], mat.m[1][0], mat.m[2][0], 0,
    mat.m[0][1], mat.m[1][1], mat.m[2][1], 0,
    mat.m[0][2], mat.m[1][2], mat.m[2][2], 0,
    mat.m[0][3], mat.m[1][3], mat.m[2][3], 1);
}

glm::vec3 dg::OVR2GLM(vr::HmdVector3_t vec) {
  return glm::vec3(vec.v[0], vec.v[1], vec.v[2]);
}
