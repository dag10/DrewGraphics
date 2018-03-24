//
//  MathUtils.cpp
//

#include <MathUtils.h>

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

glm::vec2 dg::VectorToPitchYaw(glm::vec3 direction) {
  if (direction.y == 1 || direction.y == -1) {
    direction = glm::normalize(glm::vec3(0, direction.y, -0.0001f));
  }

  glm::vec2 eulerOrientation(0);

  float horizontalLen = glm::length(glm::vec2(direction.x, direction.z));
  eulerOrientation.x = atan(direction.y / horizontalLen);

  eulerOrientation.y = -atan(direction.x / -direction.z);
  if (direction.z >= 0) {
    eulerOrientation.y += glm::pi<float>();
  }

  return eulerOrientation;
}
