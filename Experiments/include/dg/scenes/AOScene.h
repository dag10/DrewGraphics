//
//  scenes/AOScene.h
//

#pragma once

#include <memory>
#include <vector>
#include "dg/Scene.h"

namespace dg {

  class Model;
  class PointLight;
  class SpotLight;

  class AOScene : public Scene {

    public:

      class LayerMask : public Scene::LayerMask {
        public:
          static inline LayerMask Overlay() { return Ord(1); }
        protected:
          LayerMask(Scene::LayerMask mask) : Scene::LayerMask(mask) {}
        private:
          static inline LayerMask Ord(uint64_t idx) {
            return LayerMask(Scene::LayerMask::Ord(HighestIndex + idx));
          }
      };

      static std::unique_ptr<AOScene> Make();

      AOScene();

      virtual void Initialize();
      virtual void Update();

    private:

      enum class OverlayState {
        None,
        GBuffer,
        Lighting,
      };

      void InitializeSubrenders();
      void CreateGBuffer();

      virtual void RenderFramebuffers();
      virtual void PreRender();

      OverlayState overlayState = OverlayState::GBuffer;

      Subrender geometrySubrender;

      std::vector<std::shared_ptr<Model>> overlayQuads;
      std::shared_ptr<Model> finalRenderQuad;

      std::shared_ptr<PointLight> pointlight;
      std::shared_ptr<SpotLight> flashlight;
      std::shared_ptr<SceneObject> skylights;

  }; // class AOScene

} // namespace dg
