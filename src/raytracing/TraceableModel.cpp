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
  return RayResult(ray, 0); // TODO
}

