//
//  raytracer/Rays.h
//

#pragma once

#include <Lights.h>
#include <Mesh.h>
#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace dg {

  struct Mesh;
  struct RayResult;
  class TraceableModel;

  class Ray {

    public:

      glm::vec3 origin;
      glm::vec3 direction;
      float refractiveIndex = 1; // air
      float scaleFromParent = 1;

      Ray() = default;
      Ray(glm::vec3 origin, glm::vec3 direction)
          : origin(origin), direction(direction) {}

      Ray TransformedBy(glm::mat4 xf) const;

      RayResult IntersectTriangle(glm::vec3 v1, glm::vec3 v2,
                                  glm::vec3 v3) const;
      RayResult IntersectMesh(std::shared_ptr<Mesh> mesh) const;
      RayResult IntersectSphere(float radius) const;
      RayResult IntersectLight(const Light::ShaderData& lightData) const;

      bool operator==(const Ray& rhs) const;
      bool operator!=(const Ray& rhs) const;
  };

  class RayResult {

    public:

      const TraceableModel *model = nullptr;
      bool hit = false;
      bool leavingSurface = false;
      std::unordered_map<int, bool> lightDirectIllumination;
      Ray ray;
      float distance = 0;
      glm::vec3 normal;
      Vertex interpolatedVertex = Vertex(Vertex::AttrFlag::NONE);

      static RayResult Hit(Ray ray, float distance, glm::vec3 normal);
      static RayResult Miss(Ray ray);

      RayResult() = default;

      static const RayResult& Closest(const RayResult& a, const RayResult& b);

      glm::vec3 GetIntersectionPoint() const;
      Ray GetReflectedRay(bool flipNormal = false) const;
      Ray GetRefractedRay() const;

      // Expects RayResult to be in Scene Space.
      Ray RayToLight(const Light::ShaderData& lightData) const;

      RayResult TransformedBy(glm::mat4 xf,
                              const Ray *substituteRay = nullptr) const;

  };

} // namespace dg
