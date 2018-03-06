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
  RayResult res = ray_MS.IntersectMesh(mesh);

  // Transform distance scalar from model space to world space before
  // returning. Also substitutes model-space ray for original world-space
  // ray.
  res.distance /= ray_MS.scaleFromParent;
  res.ray = ray;
  return res;

  return RayResult::Miss(ray);
}

