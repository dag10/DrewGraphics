//
//  VRUtils.h
//

#pragma once

#include <openvr.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace dg {

  glm::mat4x4 OVR2GLM(vr::HmdMatrix44_t mat);
  glm::mat4x4 OVR2GLM(vr::HmdMatrix34_t mat);
  glm::vec3 OVR2GLM(vr::HmdVector3_t vec);

}  // namespace dg
