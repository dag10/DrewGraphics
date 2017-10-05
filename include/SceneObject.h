//
//  SceneObject.h
//
#pragma once

#include <Transform.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace dg {

  class SceneObject {

    public:
      dg::Transform transform = Transform();

      SceneObject() = default;
      SceneObject(Transform transform);

      void LookAtDirection(glm::vec3 direction);
      void LookAtPoint(glm::vec3 target);
      void OrientUpwards();

  }; // class SceneObject

} // namespace dg
