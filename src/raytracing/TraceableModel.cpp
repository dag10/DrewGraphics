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

  RayResult res;

  if (mesh == Mesh::Sphere) {
    res = ray_MS.IntersectSphere(0.5f);
  } else if (mesh == Mesh::Quad) {
    const float size = 0.5f;
    const glm::vec3 quadVertices[] = {
      glm::vec3(-size, -size, 0),
      glm::vec3(+size, -size, 0),
      glm::vec3(+size, +size, 0),
      glm::vec3(-size, +size, 0),
    };

    RayResult firstTriangle = ray_MS.IntersectTriangle(
        quadVertices[0], quadVertices[1], quadVertices[2]);
    RayResult secondTriangle = ray_MS.IntersectTriangle(
        quadVertices[2], quadVertices[3], quadVertices[0]);

    res = RayResult::Closest(firstTriangle, secondTriangle);
  }

  // Transform distance scalar from model space to world space before
  // returning. Also substitutes model-space ray for original world-space
  // ray.
  res.distance /= glm::length(ray_MS.direction);
  res.ray = ray;
  return res;

  return RayResult::Miss(ray);
}

