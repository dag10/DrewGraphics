//
//  TraceableModel.cpp
//

#include <raytracing/TraceableModel.h>

dg::TraceableModel::TraceableModel() : Model() {}

dg::TraceableModel::TraceableModel(
    std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material,
    Transform transform) : Model(mesh, material, transform) { }

dg::TraceableModel::TraceableModel(TraceableModel& other) : Model(other) { }

void dg::TraceableModel::CacheTransforms() {
  xfSceneSpace = SceneSpace().ToMat4();
  xfSceneSpaceInv = glm::inverse(xfSceneSpace);
}

dg::RayResult dg::TraceableModel::RayTest(Ray ray) const {
  Ray ray_MS = ray.TransformedBy(xfSceneSpaceInv);
  RayResult res = ray_MS.IntersectMesh(mesh);

  // Transform distance scalar from model space to world space before
  // returning. Also substitutes model-space ray for original world-space
  // ray.
  res.distance /= ray_MS.scaleFromParent;
  res.ray = ray;
  return res;

  return RayResult::Miss(ray);
}

