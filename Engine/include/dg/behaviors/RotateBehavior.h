//
//  behaviors/RotateBehavior.h
//

#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "dg/Behavior.h"
#include "dg/Transform.h"

namespace dg {

  class RotateBehavior : public Behavior {

    public:

      // Speed is radians per second.
      // Axis is in object's local coordinate space.
      RotateBehavior(float speed, glm::vec3 axis = UP);

      virtual void Update();

    private:

      float speed;
      glm::vec3 axis;

  }; // class RotateBehavior

} // namespace dg
