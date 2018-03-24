//
//  MathUtils.h
//
#pragma once

#include <glm/glm.hpp>
#include <openvr.h>
#include <glm/gtc/matrix_transform.hpp>

namespace dg {

  glm::mat4x4 HmdMat2Glm(vr::HmdMatrix44_t hmdMat);

  glm::mat4x4 HmdMat2Glm(vr::HmdMatrix34_t hmdMat);

  glm::vec2 VectorToPitchYaw(glm::vec3 direction);

} // namespace dg
