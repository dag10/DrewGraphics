//
//  Camera.h
//
#pragma once

#include "Transform.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace dg {

  class Camera {

    public:
      dg::Transform transform;
      float fov = 60.f;
      float nearClip = 0.1f;
      float farClip = 100.f;

      Camera() = default;

      void LookAtDirection(glm::vec3 direction);
      void LookAtPoint(glm::vec3 target);

      glm::mat4x4 GetViewMatrix() const;
      glm::mat4x4 GetProjectionMatrix(float aspectRatio) const;

  }; // class Camera

} // namespace dg
