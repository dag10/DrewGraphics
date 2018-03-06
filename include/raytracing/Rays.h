//
//  raytracer/Rays.h
//

#pragma once

#include <glm/glm.hpp>

namespace dg {

  struct RayResult;

  struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;

    Ray() = default;
    Ray(glm::vec3 origin, glm::vec3 direction)
      : origin(origin), direction(direction) {}

    RayResult IntersectTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);
    RayResult IntersectSphere(float radius);
  };

  struct RayResult {
    bool hit = false;
    Ray ray;
    float distance = 0;

    static RayResult Hit(Ray ray, float distance);
    static RayResult Miss(Ray ray);

    static const RayResult& Closest(const RayResult& a, const RayResult& b);

    RayResult() = default;
  };

} // namespace dg
