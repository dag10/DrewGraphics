//
//  Camera.h
//
#pragma once

#include <SceneObject.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace dg {

  class Camera : public SceneObject {

    public:
      float fov = 60.f;
      float nearClip = 0.1f;
      float farClip = 100.f;

      Camera();

      glm::mat4x4 GetViewMatrix() const;
      glm::mat4x4 GetProjectionMatrix(float aspectRatio) const;

  }; // class Camera

} // namespace dg
