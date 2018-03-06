//
//  TraceableModel.cpp
//

#include <raytracing/TraceableModel.h>

dg::TraceableModel::TraceableModel() : Model() {}

dg::TraceableModel::TraceableModel(
    std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material,
    Transform transform) : Model(mesh, material, transform) { }

dg::TraceableModel::TraceableModel(TraceableModel& other) : Model(other) { }

dg::RayResult dg::TraceableModel::RayTest(Ray ray) const {
  Transform ss = SceneSpace();
  glm::mat4x4 xf = ss.ToMat4();
  glm::mat4x4 xfInv = glm::inverse(xf);

  Ray ray_MS = ray.TransformedBy(SceneSpace().ToMat4());
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
  res.distance /= ray_MS.scaleFromParent;
  res.ray = ray;
  return res;

  return RayResult::Miss(ray);
}

