//
//  behaviors/CaveBehavior.h
//

#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include "cavr/Cave.h"
#include "dg/Behavior.h"

namespace dg {
  class SceneObject;
  class StandardMaterial;
} // namespace dg

namespace cavr {

  class CaveBehavior : public dg::Behavior {

    public:

      virtual void Initialize();
      virtual void Start();
      virtual void Update();

      void SetVelocity(glm::vec3 velocity);
      void AddVelocity(glm::vec3 velocity);
      glm::vec3 GetVelocity() const;

      void SetShowKnots(bool showKnots);
      bool GetShowKnots() const;

      void SetShowWireframe(bool showWireframe);
      bool GetShowWireframe() const;

      inline std::weak_ptr<dg::SceneObject> GetInteriorModels() const {
        return caveInteriorModels;
      }

      void AddNextCaveSegment();

    private:


      void AddNextCaveSegment(const CaveSegment::KnotSet &knotSet);
      void AddCaveSegment(const CaveSegment &segment);

      static CaveSegment::KnotSet CreateArcKnots();
      static CaveSegment::KnotSet CreateStraightKnots();

      glm::vec3 velocity = glm::vec3(0);
      bool showKnots = false;
      bool showWireframe = false;

      std::unique_ptr<CaveSegment> lastCaveSegment = nullptr;
      std::vector<CaveSegment::KnotSet> knotSets;

      std::shared_ptr<dg::SceneObject> knots;
      std::shared_ptr<dg::SceneObject> caveInteriorModels;
      std::shared_ptr<dg::SceneObject> caveWireframeModels;
      std::shared_ptr<dg::SceneObject> caveTransparentModels;

      std::shared_ptr<dg::SceneObject> CreateKnotVertexVisualization(
          const CaveSegment::Knot &knot) const;
      std::shared_ptr<dg::SceneObject> CreateKnotVisualization(
          const CaveSegment::Knot &knot) const;

      std::shared_ptr<dg::StandardMaterial> caveMaterial;
      std::shared_ptr<dg::StandardMaterial> caveTransparentMaterial;
      std::shared_ptr<dg::StandardMaterial> caveWireframeMaterial;
      std::shared_ptr<dg::StandardMaterial> ringVertexMaterial;
      std::shared_ptr<dg::StandardMaterial> knotDiskMaterial;
      std::shared_ptr<dg::StandardMaterial> knotArrowMaterial;

  }; // class CaveBehavior

} // namespace cavr
