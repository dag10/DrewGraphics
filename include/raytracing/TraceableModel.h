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

      void CacheTransforms();

      RayResult RayTest(Ray ray) const;

    private:

      glm::mat4 xfSceneSpace;
      glm::mat4 xfSceneSpaceInv;

  }; // class TraceableModel

} // namespace dg
