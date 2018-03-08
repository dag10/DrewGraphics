//
//  Camera.h
//
#pragma once

#include "dg/SceneObject.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <openvr.h>

namespace dg {

  class Camera : public SceneObject {

    public:
      float aspectRatio = 1;
      float fov = glm::radians(60.f); // radians
      float nearClip = 0.1f;
      float farClip = 100.f;

      Camera();

      glm::mat4x4 GetViewMatrix() const;
      glm::mat4x4 GetViewMatrix(vr::EVREye eye) const;
      glm::mat4x4 GetProjectionMatrix() const;
      glm::mat4x4 GetProjectionMatrix(vr::EVREye eye) const;

  }; // class Camera

} // namespace dg
