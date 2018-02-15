//
//  raytracer/Rays.h
//

#pragma once

#include <glm/glm.hpp>

namespace dg {

  struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;

    Ray() = default;
    Ray(glm::vec3 origin, glm::vec3 direction)
      : origin(origin), direction(direction) {}
  };

  struct RayResult {
    bool hit = false;
    Ray ray;
    float distance;

    static RayResult Hit(Ray ray, float distance) {
      RayResult res;
      res.ray = ray;
      res.hit = true;
      res.distance = distance;
      return res;
    }

    static RayResult Miss(Ray ray) {
      RayResult res;
      res.ray = ray;
      res.hit = false;
      return res;
    }

    RayResult() = default;
  };

} // namespace dg
