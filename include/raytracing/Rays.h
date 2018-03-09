//
//  raytracer/Rays.h
//

#pragma once

#include <glm/glm.hpp>
#include <memory>

namespace dg {

  struct Mesh;
  struct RayResult;
  class TraceableModel;

  struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
    float scaleFromParent = 1;

    Ray() = default;
    Ray(glm::vec3 origin, glm::vec3 direction)
      : origin(origin), direction(direction) {}

    Ray TransformedBy(glm::mat4 xf) const;

    RayResult IntersectTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) const;
    RayResult IntersectMesh(std::shared_ptr<Mesh> mesh) const;
    RayResult IntersectSphere(float radius) const;
  };

  struct RayResult {
    const TraceableModel *model = nullptr;
    bool hit = false;
    Ray ray;
    float distance = 0;
    glm::vec3 normal;

    static RayResult Hit(Ray ray, float distance, glm::vec3 normal);
    static RayResult Miss(Ray ray);

    static const RayResult& Closest(const RayResult& a, const RayResult& b);

    RayResult TransformedBy(glm::mat4 xf,
                            const Ray *substituteRay = nullptr) const;

    RayResult() = default;
  };

} // namespace dg
