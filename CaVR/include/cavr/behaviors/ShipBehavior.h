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
} // namespace dg

namespace cavr {

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

      inline dg::Scene::Subrender &GetIntersectionSubrender() {
        return intersectionSubrender;
	  }

      inline dg::Scene::Subrender &GetIntersectionDownscaleSubrender() {
        return intersectionDownscaleSubrender;
	  }

	  void DrawIntersectionDownscale();
	  void GenerateIntersectionMips();
	  void ReadIntersectionResults();

    private:

      glm::vec3 velocity = glm::vec3(0);
	  bool intersectsCave = false;

	  dg::Scene::Subrender intersectionSubrender;
	  dg::Scene::Subrender intersectionDownscaleSubrender;
      std::shared_ptr<dg::Texture> intersectionReadStagingTexture;
      std::shared_ptr<dg::Model> intersectionDownscaleModel;

      std::shared_ptr<dg::Model> hullSphere;

  }; // class ShipBehavior

} // namespace cavr
