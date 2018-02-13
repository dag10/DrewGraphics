//
//  raytracing/TraceableModel.h
//
#pragma once

#include <memory>
#include <Model.h>
#include <raytracing/Rays.h>

namespace dg {

  class TraceableModel : public Model {

    public:

      TraceableModel();

      TraceableModel(
          std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material,
          Transform transform);

      TraceableModel(TraceableModel& other);

      RayResult RayTest(Ray ray) const;

  }; // class TraceableModel

} // namespace dg
