//
//  TraceableModel.cpp
//

#include <raytracing/TraceableModel.h>
#include <glm/gtc/matrix_access.hpp>

dg::TraceableModel::TraceableModel() : Model() {}

dg::TraceableModel::TraceableModel(
    std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material,
    Transform transform) : Model(mesh, material, transform) { }

dg::TraceableModel::TraceableModel(TraceableModel& other) : Model(other) { }

// TODO: Refactor this function elsewhere.
// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm#C++_Implementation
// FIXME: This doesn't work correctly for the backside of triangles.
static dg::RayResult IntersectTriangle(
    dg::Ray ray, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
  using namespace dg;

  const float EPSILON = 0.0000001f;
  glm::vec3 edge1 = v2 - v1;
  glm::vec3 edge2 = v3 - v1;

  glm::vec3 h = glm::cross(ray.direction, edge2);
  float a = glm::dot(edge1, h);
  if (a > -EPSILON && a < EPSILON) {
    return RayResult::Miss(ray);
  }

  float f = 1/a;
  glm::vec3 s = ray.origin - v1;
  float u = f * glm::dot(s, h);
  if (u < 0.0 || u > 1.0) {
    return RayResult::Miss(ray);
  }

  glm::vec3 q = glm::cross(s, edge1);
  float v = f * glm::dot(ray.direction, q);
  if (v < 0.0 || u + v > 1.0) {
    return RayResult::Miss(ray);
  }

  float t = f * glm::dot(edge2, q);
  if (t > EPSILON) { // ray intersection
    return RayResult::Hit(ray, t);
  }

  return RayResult::Miss(ray);
}

dg::RayResult dg::TraceableModel::RayTest(Ray ray) const {
  Transform ss = SceneSpace();
  glm::mat4x4 xf = ss.ToMat4();
  glm::mat4x4 xfInv = glm::inverse(xf);

  // Transform ray to model space.
  glm::vec4 origin_SS_h(
      ray.origin.x,
      ray.origin.y,
      ray.origin.z,
      1);
  glm::vec4 direction_SS_h(
      ray.direction.x,
      ray.direction.y,
      ray.direction.z,
      0);
  glm::mat4x4 mat_SS(
      origin_SS_h,
      direction_SS_h,
      glm::vec4(0),
      glm::vec4(0));
  glm::mat4x4 mat_MS = xfInv * mat_SS;
  glm::vec4 origin_MS_h = glm::column(mat_MS, 0);
  glm::vec4 direction_pos_MS_h = glm::column(mat_MS, 1);
  glm::vec3 origin_MS(
      origin_MS_h.x / origin_MS_h.w,
      origin_MS_h.y / origin_MS_h.w,
      origin_MS_h.z / origin_MS_h.w);
  glm::vec3 direction_MS(
      direction_pos_MS_h.x,
      direction_pos_MS_h.y,
      direction_pos_MS_h.z);
  Ray ray_MS = ray;
  ray_MS.origin = origin_MS;
  ray_MS.direction = glm::normalize(direction_MS);

  if (mesh == Mesh::Sphere) {
    float dot = glm::dot(
        -ray_MS.origin,
        ray_MS.direction);
    glm::vec3 closest = ray_MS.origin + (dot * ray_MS.direction);

    float dist = glm::distance(closest, glm::vec3(0));

    if (dist <= 0.5f) {
      float dist_WS = dist / glm::length(ray_MS.direction);
      return RayResult::Hit(ray, dist_WS);
    }
  } else if (mesh == Mesh::Quad) {
    const float size = 0.5f;
    const glm::vec3 quadVertices[] = {
      glm::vec3(-size, -size, 0),
      glm::vec3(+size, -size, 0),
      glm::vec3(+size, +size, 0),
      glm::vec3(-size, +size, 0),
    };

    RayResult firstTriangle = IntersectTriangle(
        ray_MS, quadVertices[0], quadVertices[1], quadVertices[2]);
    RayResult secondTriangle = IntersectTriangle(
        ray_MS, quadVertices[2], quadVertices[3], quadVertices[0]);

    RayResult res = firstTriangle;
    if (!res.hit || res.distance < secondTriangle.distance) {
      res = secondTriangle;
    }

    // Transform distance scalar from model space to world space before
    // returning. Also substitutes model-space ray for original world-space
    // ray.
    res.distance /= glm::length(ray_MS.direction);
    res.ray = ray;
    return res;
  }

  return RayResult::Miss(ray);
}

