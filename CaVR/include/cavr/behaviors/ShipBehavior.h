//
//  behaviors/ShipBehavior.h
//

#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "dg/Behavior.h"
#include "dg/Scene.h"

namespace dg {
  class SceneObject;
  class StandardMaterial;
  class Model;
  class VRControllerState;
} // namespace dg

namespace cavr {

  class CaveBehavior;

  class ShipBehavior : public dg::Behavior {

    public:

      ShipBehavior(std::shared_ptr<dg::Model> hullSphere);

      virtual void Initialize();
      virtual void Start();
      virtual void Update();

      inline void SetVelocity(glm::vec3 velocity) { this->velocity = velocity; }
      inline void AddVelocity(glm::vec3 velocity) {
        this->velocity += velocity;
      }
      inline glm::vec3 GetVelocity() const { return velocity; }

      inline void SetCave(std::shared_ptr<CaveBehavior> cave) {
        this->cave = cave;
      }
      inline std::shared_ptr<CaveBehavior> GetCave() const {
        return cave.lock();
      }

      inline void SetControllerState(
          std::shared_ptr<dg::VRControllerState> controllerState) {
        this->controllerState = controllerState;
      }
      inline std::shared_ptr<dg::VRControllerState> GetControllerState() const {
        return controllerState.lock();
      }

      inline dg::Scene::Subrender &GetIntersectionSubrender() {
        return intersectionSubrender;
      }

      inline dg::Scene::Subrender &GetIntersectionDownscaleSubrender() {
          return intersectionDownscaleSubrender;
      }

      inline bool IntersectsCave() const { return intersectsCave; }

      void DrawIntersectionDownscale();
      void GenerateIntersectionMips();
      void ReadIntersectionResults();

    private:

      glm::vec3 velocity = glm::vec3(0);
      bool intersectsCave = false;

      std::weak_ptr<CaveBehavior> cave;
      std::weak_ptr<dg::VRControllerState> controllerState;

      dg::Scene::Subrender intersectionSubrender;
      dg::Scene::Subrender intersectionDownscaleSubrender;
      std::shared_ptr<dg::Texture> intersectionReadStagingTexture;
      std::shared_ptr<dg::Model> intersectionDownscaleModel;

      std::shared_ptr<dg::Model> hullSphere;

  }; // class ShipBehavior

} // namespace cavr
