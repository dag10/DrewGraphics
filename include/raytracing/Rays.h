//
//  raytracer/Rays.h
//

#pragma once

#include <glm/glm.hpp>

namespace dg {

  struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;

    Ray(glm::vec3 origin, glm::vec3 direction)
      : origin(origin), direction(direction) {}
  };

  struct RayResult {
    Ray ray;
    float distance;

    RayResult(Ray ray, float distance)
      : ray(ray), distance(distance) {}
  };

} // namespace dg
