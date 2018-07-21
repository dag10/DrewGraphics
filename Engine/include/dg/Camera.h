//
//  Camera.h
//
#pragma once

#include <openvr.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "dg/SceneObject.h"
#include "dg/Texture.h"

namespace dg {

  class Camera : public SceneObject {

    public:

      enum class Projection {
        Perspective,
        Orthographic,
      };

      Projection projection = Projection::Perspective;
      float aspectRatio = 1;
      float fov = glm::radians(60.f); // radians
      float nearClip = 0.1f;
      float farClip = 100.f;
      float orthoWidth = 0.1f;
      float orthoHeight = 0.1f;

      Camera();

      glm::mat4x4 GetViewMatrix() const;
      glm::mat4x4 GetViewMatrix(vr::EVREye eye) const;
      glm::mat4x4 GetViewMatrix(TextureFace face) const;
      glm::mat4x4 GetProjectionMatrix() const;
      glm::mat4x4 GetProjectionMatrix(vr::EVREye eye) const;

  }; // class Camera

} // namespace dg
