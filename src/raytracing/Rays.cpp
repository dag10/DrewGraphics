//
//  raytracer/Rays.cpp
//

#include <raytracing/Rays.h>
#include <glm/gtc/matrix_access.hpp>

dg::Ray dg::Ray::TransformedBy(glm::mat4 xf) const {
  glm::mat4 xfInv = glm::inverse(xf);

  // Transform ray to model space.
  glm::vec4 origin_SS_h(
      origin.x,
      origin.y,
      origin.z,
      1);
  glm::vec4 direction_SS_h(
      direction.x,
      direction.y,
      direction.z,
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

  Ray ret = *this;
  ret.scaleFromParent = glm::length(direction_MS);
  ret.origin = origin_MS;
  ret.direction = glm::normalize(direction_MS);
  return ret;
}

// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm#C++_Implementation
dg::RayResult dg::Ray::IntersectTriangle(
    glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) const {

  const float EPSILON = 0.0000001f;
  glm::vec3 edge1 = v2 - v1;
  glm::vec3 edge2 = v3 - v1;

  glm::vec3 h = glm::cross(direction, edge2);
  float a = glm::dot(edge1, h);
  if (a > -EPSILON && a < EPSILON) {
    return RayResult::Miss(*this);
  }

  float f = 1/a;
  glm::vec3 s = origin - v1;
  float u = f * glm::dot(s, h);
  if (u < 0.0 || u > 1.0) {
    return RayResult::Miss(*this);
  }

  glm::vec3 q = glm::cross(s, edge1);
  float v = f * glm::dot(direction, q);
  if (v < 0.0 || u + v > 1.0) {
    return RayResult::Miss(*this);
  }

  float t = f * glm::dot(edge2, q);
  if (t > EPSILON) { // ray intersection
    return RayResult::Hit(*this, t);
  }

  return RayResult::Miss(*this);
}

dg::RayResult dg::Ray::IntersectSphere(float radius) const {
  float dot = glm::dot(-origin, direction);
  glm::vec3 closest = origin + (dot * direction);

  float dist = glm::distance(closest, glm::vec3(0));

  if (dist <= radius) {
    return RayResult::Hit(*this, glm::distance(closest, origin));
  }

  return RayResult::Miss(*this);
}

dg::RayResult dg::RayResult::Hit(Ray ray, float distance) {
  RayResult res;
  res.ray = ray;
  res.hit = true;
  res.distance = distance;
  return res;
}

dg::RayResult dg::RayResult::Miss(Ray ray) {
  RayResult res;
  res.ray = ray;
  res.hit = false;
  return res;
}

const dg::RayResult& dg::RayResult::Closest(
    const RayResult& a, const RayResult& b) {
  return (!b.hit || (a.hit && a.distance < b.distance)) ? a : b;
}

