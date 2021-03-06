//
//  scenes/GameScene.h
//

#pragma once

#include <cassert>
#include <memory>
#include <vector>
#include "dg/FrameBuffer.h"
#include "dg/Mesh.h"
#include "dg/Scene.h"

namespace dg {
  class SceneObject;
  class Model;
  class DirectionalLight;
  class PointLight;
  class Camera;
  class StandardMaterial;
} // namespace dg

namespace cavr {

  class ShipBehavior;

  class GameScene : public dg::Scene {

    public:

      static std::unique_ptr<GameScene> Make();

      virtual void Initialize();
      virtual void Update();

      class LayerMask : public dg::Scene::LayerMask {
        public:
          // Layer used for inner cave wall geometry to detect cave
          // intersections using the GPU.
          static inline LayerMask CaveGeometry() { return Ord(1); }
          static inline LayerMask StartGeometry() { return Ord(2); }
        protected:
          LayerMask(Scene::LayerMask mask) : Scene::LayerMask(mask) {}
        private:
          static inline LayerMask Ord(uint64_t idx) {
            return LayerMask(Scene::LayerMask::Ord(HighestIndex + idx));
          }
      };

    private:

      static std::shared_ptr<dg::SceneObject> CreateShip();

      enum class GameState {
        Start,
        Starting,
        Playing,
        Dead,
      };

      enum class DevModeState {
        Disabled,

        // Both controller's menu buttons are pressed, waiting on both of
        // them to be released.
        AwaitingRelease,

        // Developer-only buttons on controllers are enabled, such as toggling
        // the cave wireframe, etc.
        Enabled,
      };

      GameScene();

      virtual void RenderFramebuffers();
      virtual void DrawCustomSubrender(const Subrender &subrender);
      virtual void PostProcess();
      virtual void ResourceReadback();

      void SwapControllers();

      void ResetGame();
      void StartGame();
      void PlayerDied();

      float startCountdown = 0;
      float speedRampUp = 0;
      float elapsedTime = 0;
      dg::Transform caveStartTransform;

      DevModeState devModeState = DevModeState::Disabled;
      GameState gameState = GameState::Start;
      std::shared_ptr<dg::Model> renderQuad;

      std::shared_ptr<dg::SceneObject> leftController;
      std::shared_ptr<dg::SceneObject> rightController;

      std::shared_ptr<dg::SceneObject> shipAttachment;
      std::shared_ptr<ShipBehavior> ship;
      std::shared_ptr<dg::DirectionalLight> skyLight;
      std::shared_ptr<dg::Model> floor;
      std::shared_ptr<dg::Model> startModel;
      std::shared_ptr<dg::SceneObject> cave;

  }; // class GameScene

} // namespace cavr
