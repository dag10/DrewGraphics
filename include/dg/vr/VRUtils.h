//
//  VRUtils.h
//

#pragma once

#include <openvr.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace dg {

  glm::mat4x4 OVR2GLM(vr::HmdMatrix44_t hmdMat);
  glm::mat4x4 OVR2GLM(vr::HmdMatrix34_t hmdMat);

}  // namespace dg
