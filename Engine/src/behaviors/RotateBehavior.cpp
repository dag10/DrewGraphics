//
//  behaviors/RotateBehavior.cpp
//

#include "dg/behaviors/RotateBehavior.h"
#include <glm/gtc/quaternion.hpp>
#include "dg/EngineTime.h"
#include "dg/SceneObject.h"

// Speed is radians per second.
// Axis is in object's local coordinate space.
dg::RotateBehavior::RotateBehavior(float speed, glm::vec3 axis)
    : speed(speed), axis(axis) {}

void dg::RotateBehavior::Update() {
  Behavior::Update();

  std::shared_ptr<SceneObject> obj = GetSceneObject();
  if (!obj) return;

  obj->transform.rotation = glm::rotate(
      obj->transform.rotation, speed * (float)Time::Delta, axis);
}
