//
//  scenes/GameScene.h
//

#pragma once

#include <cassert>
#include <memory>
#include <vector>
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

  class GameScene : public dg::Scene {

    public:

      static std::unique_ptr<GameScene> Make();

      virtual void Initialize();
      virtual void Update();

    private:

      GameScene();

      std::shared_ptr<SceneObject> leftController;
      std::shared_ptr<SceneObject> rightController;

      std::shared_ptr<dg::PointLight> controllerLight;
      std::shared_ptr<dg::DirectionalLight> skyLight;
      std::shared_ptr<dg::Model> floor;
      std::shared_ptr<dg::SceneObject> cave;

  }; // class GameScene

} // namespace cavr
