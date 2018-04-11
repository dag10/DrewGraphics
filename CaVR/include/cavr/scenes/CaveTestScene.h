//
//  scenes/CaveTestScene.h
//

#pragma once

#include <cassert>
#include <memory>
#include <vector>
#include "cavr/Cave.h"
#include "dg/Mesh.h"
#include "dg/Scene.h"

namespace dg {
  class SceneObject;
  class Model;
  class DirectionalLight;
  class PointLight;
  class StandardMaterial;
} // namespace dg

namespace cavr {

  class CaveTestScene : public dg::Scene {

    public:

      static std::unique_ptr<CaveTestScene> Make();

      virtual void Initialize();
      virtual void Update();

    private:

      CaveTestScene();

      static CaveSegment::KnotSet CreateArcKnots();

      void CreateCave();
      std::shared_ptr<dg::SceneObject> CreateKnotVertexModels(
          const CaveSegment::Knot &knot) const;
      std::shared_ptr<dg::SceneObject> CreateKnotModels(
          const CaveSegment::Knot &knot) const;

      std::shared_ptr<dg::PointLight> controllerLight;
      std::shared_ptr<dg::DirectionalLight> skyLight;
      std::shared_ptr<dg::Model> floor;
      std::shared_ptr<dg::SceneObject> caveContainer;
      std::shared_ptr<dg::SceneObject> knots;

      std::shared_ptr<dg::StandardMaterial> caveMaterial;
      std::shared_ptr<dg::StandardMaterial> caveTransparentMaterial;
      std::shared_ptr<dg::StandardMaterial> caveWireframeMaterial;
      std::shared_ptr<dg::StandardMaterial> ringVertexMaterial;
      std::shared_ptr<dg::StandardMaterial> knotDiskMaterial;
      std::shared_ptr<dg::StandardMaterial> knotArrowMaterial;

  }; // class CaveTestScene

} // namespace cavr
